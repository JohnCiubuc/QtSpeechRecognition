#include "qtspeechrecognition.h"
#define db qDebug() << this <<
QtSpeechRecognition::QtSpeechRecognition(float micThreshold)
{
  fMicThreshold = micThreshold;
  QString MODELDIR = QDir::currentPath().append("/model");
  Config = cmd_ln_init(NULL, ps_args(), TRUE,
                       "-hmm", (MODELDIR + "/en-us/en-us").toLocal8Bit().data(),
                       "-lm", (MODELDIR + "/en-us/en-us.lm.bin").toLocal8Bit().data(),
                       "-dict", (MODELDIR + "/en-us/cmudict-en-us.dict").toLocal8Bit().data(),
//                       "-logfn", "/dev/null",                                      // suppress log info from being sent to screen
                       NULL);

  if (Config == NULL)
  {
    fprintf(stderr, "Failed to create config object, see log for details\n");
    return ;
  }

  SphinxDecoder = ps_init(Config);

  if (SphinxDecoder == NULL)
  {
    fprintf(stderr, "Failed to create recognizer, see log for details\n");
    return ;
  }

  // Platform Specific!!
  AudioRecorder = ad_open();
  MicrophoneTimer = new QTimer(this);
  connect(MicrophoneTimer, &QTimer::timeout, this, &QtSpeechRecognition::decodeMicrophone);
  MicrophoneListener = new QTimer(this);
  connect(MicrophoneListener, &QTimer::timeout, this, &QtSpeechRecognition::decodeSpeech);
  // Initiliaze Audio / Mic to determine Mic Levels
  initializeAudio(QAudioDeviceInfo::defaultInputDevice());
}

QtSpeechRecognition::~QtSpeechRecognition()
{
  ad_close(AudioRecorder);
  ps_free(SphinxDecoder);
  cmd_ln_free_r(Config);
}

void QtSpeechRecognition::startDebug()
{
  debugbStartCounter = true;
  debugCounter = 0;
  micTotal.clear();
  db ps_set_search(SphinxDecoder, "test");
}

void QtSpeechRecognition::stopDebug()
{
  debugbStartCounter = false;
  db b_ps_utt(false);                          // then mark the end of the utterance
  db ad_stop_rec(AudioRecorder);
  db ps_unset_search(SphinxDecoder, "test");
  db ps_set_kws(SphinxDecoder, "test2", "output2.raw");
  db ps_set_search(SphinxDecoder, "test2");
}

void QtSpeechRecognition::loadKeywords(QStringList list)
{
  b_ps_utt(false);                          // then mark the end of the utterance
  ad_stop_rec(AudioRecorder);
  ps_unset_search(SphinxDecoder, "default");
  // Requires pocketsphinx_qt.patch applied in pocketsphinx directory, and recompiled
  QString phrases = list.join("/1e-1/\n");
  phrases.append("/1e-1/");
  qDebug() << "TEST OUT: " << phrases.toLocal8Bit().data();
  ps_set_kws_mem(SphinxDecoder, "default", phrases.toLocal8Bit().data());
  ps_set_search(SphinxDecoder, "default");
}

void QtSpeechRecognition::startListening()
{
  bAllowListening = true;
  m_audioInfo->start();
  m_audioInput->start(m_audioInfo);
}

void QtSpeechRecognition::stopListening()
{
  bAllowListening = false;
  m_audioInfo->stop();
  m_audioInput->stop();
}

int QtSpeechRecognition::b_ps_utt(bool b)
{
  bStartUtt = b;
  return b ? ps_start_utt(SphinxDecoder) : ps_end_utt(SphinxDecoder);
}

void QtSpeechRecognition::decodeMicrophone()
{
  b_ps_utt(true);
  ad_start_rec(AudioRecorder);
//  bool utt_started = FALSE;                             // clear the utt_started flag
  bListening = false;
  bHypothesisProcess = false;
  MicrophoneListener->start(1);
//  db "Decode Microphone Init";
//  while(1)
//  {
//  }
}

void QtSpeechRecognition::decodeSpeech()
{
//  db "Decode Speech";
  int k = ad_read(AudioRecorder, AudioBuffer, 2048);                // capture the number of frames in the audio buffer
  ps_process_raw(SphinxDecoder, AudioBuffer, k, FALSE, FALSE);  // send the audio buffer to the pocketsphinx decoder

  if(ps_get_in_speech(SphinxDecoder) && !bListening)            // test to see if speech is being detected
  {
//    db "Start Listening";
    bListening = true;
    bHypothesisProcess = true;
//    MicrophoneTimer->stop();
//    MicrophoneListener->start(10);
  }

  if(bListening)
  {
    // stop recording
    int32 start = 0;
    ps_seg_t * iter = ps_seg_iter(SphinxDecoder);
    float highConfidence = 0;
    QString hypothesis;

    while (iter != NULL)
    {
      int32 sf, ef, pprob;
      float conf1;
      ps_seg_frames(iter, &sf, &ef);
      pprob = ps_seg_prob(iter, NULL, NULL, NULL);
      conf1 = logmath_exp(ps_get_logmath(SphinxDecoder), pprob);
      //      printf("%s %f %f %f\n", ps_seg_word(iter), (sf + start) / 100.0, (ef + start) / 100.0, conf1);
      db ps_seg_word(iter) << ", " << (sf + start) / 100.0 << ", " << (ef + start) / 100.0 << ", " << conf1;

      if(conf1 > highConfidence)
      {
        highConfidence = conf1;
        hypothesis = QString( ps_seg_word(iter));
      }

      iter = ps_seg_next(iter);
    }

    if(!hypothesis.isEmpty())
    {
      MicrophoneListener->stop();
      b_ps_utt(false);                          // then mark the end of the utterance
      ad_stop_rec(AudioRecorder);                         // stop recording
      bListening = false;
      bHypothesisProcess = false;
      db "Quick Hypo: " << hypothesis;
      emit hypothesisResult(hypothesis);
      return;
    }

//    QString Hypothesis = QString::fromLocal8Bit(ps_get_hyp(SphinxDecoder, nullptr));             // query pocketsphinx for "hypothesis" of decoded statement
  }

  // Speech ended
  if(!ps_get_in_speech(SphinxDecoder) && bListening)
  {
//    db "Stop Listening";
    b_ps_utt(false);                          // then mark the end of the utterance
    ad_stop_rec(AudioRecorder);                         // stop recording
    int32 score = 0;
    int32 start = 0;
    ps_seg_t * iter = ps_seg_iter(SphinxDecoder);
    float highConfidence = 0;
    QString hypothesis;

    while (iter != NULL)
    {
      int32 sf, ef, pprob;
      float conf1;
      ps_seg_frames(iter, &sf, &ef);
      pprob = ps_seg_prob(iter, NULL, NULL, NULL);
      conf1 = logmath_exp(ps_get_logmath(SphinxDecoder), pprob);
//      printf("%s %f %f %f\n", ps_seg_word(iter), (sf + start) / 100.0, (ef + start) / 100.0, conf1);
      db ps_seg_word(iter) << ", " << (sf + start) / 100.0 << ", " << (ef + start) / 100.0 << ", " << conf1;

      if(conf1 > highConfidence)
      {
        highConfidence = conf1;
        hypothesis = QString( ps_seg_word(iter));
      }

      iter = ps_seg_next(iter);
    }

    score = ps_get_prob(SphinxDecoder);
//    qDebug() << this << "Score: " << score << ", Hypothesis - " << Hypothesis;
    db "Final Hypothesis Victor: " << hypothesis;
    emit hypothesisResult(hypothesis);
    MicrophoneListener->stop();
    bListening = false;
    bHypothesisProcess = false;
//    decodeMicrophone();
  }
}

void QtSpeechRecognition::initializeAudio(const QAudioDeviceInfo & deviceInfo)
{
  QAudioFormat format;
  format.setSampleRate(8000);
  format.setChannelCount(1);
  format.setSampleSize(16);
  format.setSampleType(QAudioFormat::SignedInt);
  format.setByteOrder(QAudioFormat::LittleEndian);
  format.setCodec("audio/pcm");

  if (!deviceInfo.isFormatSupported(format))
  {
    qWarning() << "Default format not supported - trying to use nearest";
    format = deviceInfo.nearestFormat(format);
  }

  m_audioInfo = new AudioInfo(format);
  connect(m_audioInfo, &AudioInfo::update, this, &QtSpeechRecognition::listenMicrophoneAudioLevel);
  m_audioInput.reset(new QAudioInput(deviceInfo, format));
}

void QtSpeechRecognition::listenMicrophoneAudioLevel()
{
  if(!bAllowListening) return;

  if(debugbStartCounter)
  {
    debugCounter++;
    micTotal << m_audioInfo->level();

    if(debugCounter > 10)
    {
      debugCounter = 0;
      micTotal << debugCounter;
      float tempt = 0;

      for(auto value : micTotal)
        tempt += value;

      emit debugMicLevel(tempt / micTotal.size());
      micTotal.clear();
    }
  }

  // Starts listening to Audio if Mic Level is greater than threshold
  // Generally User's voice is louder than ambient level.
  if(m_audioInfo->level() > fMicThreshold && !bStartUtt)
    decodeMicrophone();
}
