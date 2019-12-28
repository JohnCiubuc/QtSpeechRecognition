#ifndef QTSPEECHRECOGNITION_GLOBAL_H
#define QTSPEECHRECOGNITION_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QTSPEECHRECOGNITION_LIBRARY)
#  define QTSPEECHRECOGNITION_EXPORT Q_DECL_EXPORT
#else
#  define QTSPEECHRECOGNITION_EXPORT Q_DECL_IMPORT
#endif

#endif // QTSPEECHRECOGNITION_GLOBAL_H