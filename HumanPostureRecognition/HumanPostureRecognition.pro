QT       += core gui
QT += multimedia
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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
    AI/myutils.cpp \
    AI/nanodet_dancing.cpp \
    AI/nanodet_movenet.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    AI/myutils.h \
    AI/nanodet_dancing.h \
    AI/nanodet_movenet.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    HumanPostureRecognition_zh_CN.ts
#OpenCV
INCLUDEPATH += D:\ProgramData\opencv454\opencv\build\include
INCLUDEPATH += D:\ProgramData\ncnn-20211208-windows-vs2019-shared\x64\include\ncnn
INCLUDEPATH += D:\ProgramData\VulkanSDK\1.2.148.0\Include

CONFIG(release,debug|release){
LIBS += -LD:\ProgramData\opencv454\opencv\build\x64\vc15\lib -lopencv_world454
LIBS += -LD:\ProgramData\VulkanSDK\1.2.148.0\Lib -lvulkan-1
LIBS += -LD:\ProgramData\ncnn-20211208-windows-vs2019-shared\x64\lib -lncnn

}else{
LIBS += -LD:\ProgramData\opencv454\opencv\build\x64\vc15\lib -lopencv_world454d
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
