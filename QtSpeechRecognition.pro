TEMPLATE = subdirs

SUBDIRS += \
    QtSpeechRecognition \
    DLLTestLoader

DLLTestLoader.depends = QtSpeechRecognition

DEPENDPATH += /usr/local/lib/
