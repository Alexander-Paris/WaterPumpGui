QT += core gui widgets serialport network

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    controlboardinterface.cpp \
    basestationinterface.cpp \
    maintab.cpp \
    settingstab.cpp \
    tagstab.cpp \
    welcometab.cpp \
    newbottle.cpp \
    onscreenkeypad.cpp \
    onscreenkeyboard.cpp \
    waterusagetab.cpp

HEADERS += \
    mainwindow.h \
    controlboardinterface.h \
    basestationinterface.h \
    maintab.h \
    settingstab.h \
    tagstab.h \
    welcometab.h \
    newbottle.h \
    onscreenkeypad.h \
    onscreenkeyboard.h \
    waterusagetab.h

FORMS += \
    mainwindow.ui \
    maintab.ui \
    settingstab.ui \
    tagstab.ui \
    welcometab.ui \
    newbottle.ui \
    onscreenkeypad.ui \
    onscreenkeyboard.ui \
    waterusagetab.ui

target.path = /home/root
INSTALLS += target
