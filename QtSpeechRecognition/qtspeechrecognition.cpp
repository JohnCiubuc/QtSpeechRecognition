#include "qtspeechrecognition.h"
#define db qDebug() << this <<
QtSpeechRecognition::QtSpeechRecognition(float micThreshold)
{
  fMicThreshold = micThreshold;
  QString MODELDIR = "/hdd/Sync Drive/Programming Projects/QtSpeechRecognition/QtSpeechRecognition/pocketsphinx/model";
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
//  SphinxDecoder->ps_set_ke
//  db ps_set_jsgf_file(SphinxDecoder, "test", "/hdd/Sync Drive/Programming Projects/build-QtSpeechRecognition-GCC_64-Release/DLLTestLoader/basic_command");
//  QBuffer buffer;
//  buffer.open(QBuffer::ReadWrite);
//  QFile myFile(&buffer);
//  QFile myFile("output.raw");
//  myFile.open(QIODevice::WriteOnly);
//  QTextStream os(&myFile);
//  os << "open menu /1e-1/\nselect next target /1e-1/\nactivate jump /1e-1/\nup /1e-1/";
//  myFile.close();
//  int fileHandle = myFile.handle();
//  FILE * fh = fdopen(fileHandle, "rb");
//  db ps_set_kws(SphinxDecoder, "test", "output.raw");
//  myFile.setFileName("output2.raw");
//  myFile.open(QIODevice::WriteOnly);
//  myFile.resize(0);
//  os.setDevice(&myFile);
//  os << "open menu/1e-1/\nselect next target/1e-1/\nactivate jump /1e-1/\nup /1e-1/\ntarget left /1e-1/\ntarget right /1e-1/\nshut down now /1e-1/";
//  db ps_set_kws(SphinxDecoder, "test2", "output2.raw");
//  ps_set_keyphrase(SphinxDecoder, "test", "open menu /1e-1/\nselect next target /1e-1/\nactivate jump /1e-1/\nup /1e-1/");
//  ps_set_keyphrase(SphinxDecoder, "test", "say hello to my friend /1e-30/");
//  db ps_set_search(SphinxDecoder, "test");

  if (SphinxDecoder == NULL)
  {
    fprintf(stderr, "Failed to create recognizer, see log for details\n");
    return ;
  }

  // Platform Specific!!
//  AudioRecorder =  ad_open_dev("sysdefault", (int) cmd_ln_float32_r(Config, "-samprate")); // open default microphone at default samplerate
//  db "before";
  AudioRecorder = ad_open();
//  db "after";
  MicrophoneTimer = new QTimer(this);
  connect(MicrophoneTimer, &QTimer::timeout, this, &QtSpeechRecognition::decodeMicrophone);
  MicrophoneListener = new QTimer(this);
  connect(MicrophoneListener, &QTimer::timeout, this, &QtSpeechRecognition::decodeSpeech);
  // Initiliaze Audio / Mic to determine Mic Levels
  initializeAudio(QAudioDeviceInfo::defaultInputDevice());
  // Start Listening
//  decodeMicrophone();
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
  db ps_end_utt(SphinxDecoder);                          // then mark the end of the utterance
  db ad_stop_rec(AudioRecorder);
  db ps_unset_search(SphinxDecoder, "test");
  db ps_set_kws(SphinxDecoder, "test2", "output2.raw");
  db ps_set_search(SphinxDecoder, "test2");
}

void QtSpeechRecognition::loadKeywords(QStringList list)
{
  db ps_end_utt(SphinxDecoder);                          // then mark the end of the utterance
  db ad_stop_rec(AudioRecorder);
  db ps_unset_search(SphinxDecoder, "default");
  // Temporary solution
  QFile myFile("output.raw");
  myFile.open(QIODevice::WriteOnly);
  QTextStream os(&myFile);

  for (auto s : list)
    os << s << "/1e-1/\n";

//  os << "open menu /1e-1/\nselect next target /1e-1/\nactivate jump /1e-1/\nup /1e-1/";
  myFile.close();
  //  int fileHandle = myFile.handle();
  //  FILE * fh = fdopen(fileHandle, "rb");
  db ps_set_kws(SphinxDecoder, "default", "output.raw");
  db ps_set_search(SphinxDecoder, "default");
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

void QtSpeechRecognition::decodeMicrophone()
{
  ps_start_utt(SphinxDecoder);
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
      ps_end_utt(SphinxDecoder);                          // then mark the end of the utterance
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
    ps_end_utt(SphinxDecoder);                          // then mark the end of the utterance
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
//  m_audioInfo.reset(new AudioInfo(format));
  connect(m_audioInfo, &AudioInfo::update, this, &QtSpeechRecognition::listenMicrophoneAudioLevel);
  m_audioInput.reset(new QAudioInput(deviceInfo, format));
//  m_audioInfo->start();
//  m_audioInput->start(m_audioInfo);
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

//  db "Audio Level: " << m_audioInfo->level();
  if(m_audioInfo->level() > fMicThreshold && !bHypothesisProcess)
  {
//    bListening = true;
    decodeMicrophone();
  }
}
