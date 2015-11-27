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

unix:!macx: LIBS += -L$$PWD/../../opencv_ARM/build/install/lib -lopencv_imgproc -lopencv_core -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_flann -lopencv_hal -lopencv_imgcodecs -lopencv_photo -lopencv_shape -lopencv_stitching -lopencv_superres -lopencv_ts -lopencv_videostab

INCLUDEPATH += $$PWD/../../opencv_ARM/build/install/include
DEPENDPATH += $$PWD/../../opencv_ARM/build/install/include

#3rd party packages
INCLUDEPATH += $$PWD/../../opencv_ARM/build/3rdparty
DEPENDPATH += $$PWD/../../opencv_ARM/build/3rdparty

unix:!macx: LIBS += -L$$PWD/../../opencv_ARM/build/3rdparty/lib/ -lzlib
unix:!macx: PRE_TARGETDEPS += $$PWD/../../opencv_ARM/build/3rdparty/lib/libzlib.a

unix:!macx: LIBS += -L$$PWD/../../opencv_ARM/build/3rdparty/lib/ -llibwebp
unix:!macx: PRE_TARGETDEPS += $$PWD/../../opencv_ARM/build/3rdparty/lib/liblibwebp.a

unix:!macx: LIBS += -L$$PWD/../../opencv_ARM/build/3rdparty/lib/ -lIlmImf
unix:!macx: PRE_TARGETDEPS += $$PWD/../../opencv_ARM/build/3rdparty/lib/libIlmImf.a

unix:!macx: LIBS += -L$$PWD/../../../../opt/poky-lla/1.7.1/sysroots/armv6-vfp-poky-linux-gnueabi/lib/ -ldl-2.20 -lc

INCLUDEPATH += $$PWD/../../../../opt/poky-lla/1.7.1/sysroots/armv6-vfp-poky-linux-gnueabi
DEPENDPATH += $$PWD/../../../../opt/poky-lla/1.7.1/sysroots/armv6-vfp-poky-linux-gnueabi
