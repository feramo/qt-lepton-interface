TARGET = FLIR

SOURCES += \
    Lepton_I2C.cpp \
    LeptonThread.cpp \
    main.cpp \
    MyLabel.cpp \
    Palettes.cpp \
    SPI.cpp

HEADERS += \
    Lepton_I2C.h \
    LeptonThread.h \
    MyLabel.h \
    Palettes.h \
    SPI.h

unix:!macx: LIBS += -L$$PWD/leptonSDKEmb32PUB/Debug/ -lLEPTON_SDK

INCLUDEPATH += $$PWD/leptonSDKEmb32PUB/Debug
DEPENDPATH += $$PWD/leptonSDKEmb32PUB/Debug

unix:!macx: PRE_TARGETDEPS += $$PWD/leptonSDKEmb32PUB/Debug/libLEPTON_SDK.a
