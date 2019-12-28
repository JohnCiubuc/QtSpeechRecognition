#ifndef QTSPEECHRECOGNITION_H
#define QTSPEECHRECOGNITION_H

#include "QtSpeechRecognition_global.h"

#include <QtCore/QtCore>
#include <QString>
#include <pocketsphinx/pocketsphinx.h>
#include <sphinxbase/ad.h>
#include <sphinxbase/jsgf.h>
#include <sphinxbase/err.h>
#include <QTimer>
#include <QObject>
#include <QFile>
#include <QTextStream>

#include "audioinput.h"

class QTSPEECHRECOGNITION_EXPORT QtSpeechRecognition: public QObject
{
  Q_OBJECT
signals:
  void debugMicLevel(float);
  void firstHypothesis(QString);
  void finalHypothesis(QString);
public:
  QtSpeechRecognition(float micThreshold);
  ~QtSpeechRecognition();
public slots:
  void loadKeywords(QStringList list);
  void startListening();
  void stopListening();





  void setMicThreshold(float f)
  {
    fMicThreshold = f;
  };
private slots:
  void decodeMicrophone();
  void decodeSpeech();
  void initializeAudio(const QAudioDeviceInfo & deviceInfo);
  void listenMicrophoneAudioLevel();
  int b_ps_utt(bool);
  QString getHypothesis();
private:

  AudioInfo * m_audioInfo;
  QScopedPointer<QAudioInput> m_audioInput;

  ps_decoder_t * SphinxDecoder;
  cmd_ln_t * Config;
  ad_rec_t * AudioRecorder;

  QTimer * MicrophoneTimer;
  QTimer * MicrophoneListener;

  int16 AudioBuffer[4096];

  bool bListening = false;
  bool bFinalHypothesis = false;
  bool bAllowListening = true;
  bool bStartUtt = false;

  float fMicThreshold;
  bool debugbStartCounter = false;
  int debugCounter = 0;
  QList<float> micTotal;
};

#endif // QtSpeechRECOGNITION_H
