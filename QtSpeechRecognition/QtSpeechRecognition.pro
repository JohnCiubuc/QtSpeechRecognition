QT -= gui

QT += multimedia

TEMPLATE = lib
DEFINES += QTSPEECHRECOGNITION_LIBRARY

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    audioinput.cpp \
    qtspeechrecognition.cpp

HEADERS += \
    QtSpeechRecognition_global.h \
    audioinput.h \
    qtspeechrecognition.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

INCLUDEPATH += $$PWD/pocketsphinx/include $$PWD/sphinxbase/include
DEPENDPATH += $$PWD/pocketsphinx/include $$PWD/sphinxbase/include
DEPENDPATH += /usr/local/lib/

unix:!macx: LIBS += -lsphinxbase -lsphinxad -lpocketsphinx
#unix:!macx: LIBS += -lpocketsphinx

