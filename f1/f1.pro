QT += widgets
QT += core gui widgets multimedia

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Startmenu.cpp \
    Wire.cpp \
    basecomponents.cpp \
    component.cpp \
    componentlibrary.cpp \
    main.cpp \
    maincanvas.cpp \
    mainwindow.cpp \
    pin.cpp

HEADERS += \
    Startmenu.h \
    Wire.h \
    basecomponents.h \
    component.h \
    componentlibrary.h \
    gates.h \
    maincanvas.h \
    mainwindow.h \
    pin.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    image.qrc \
    resources.qrc
    resources.qrc
