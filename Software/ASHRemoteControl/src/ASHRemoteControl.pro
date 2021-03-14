#-------------------------------------------------
#
# Project created by QtCreator 2019-01-05T13:00:17
#
#-------------------------------------------------

QT       += core gui network qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ASHRemoteControl
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

android {
    QT += androidextras
}


SOURCES += \
    casnpclient.cpp \
    cfixedslider.cpp \
        main.cpp \
        mainwindow.cpp \
    cudpclient.cpp \
    utils.cpp \
    asnp_utils.cpp

HEADERS += \
    casnpclient.h \
    cfixedslider.h \
        mainwindow.h \
    cudpclient.h \
    utils.h \
    asnp_utils.h

FORMS += \
        mainwindow.ui

CONFIG += mobility
MOBILITY = 

DISTFILES += \
    android-src/AndroidManifest.xml \
    android-src/gradle/wrapper/gradle-wrapper.jar \
    android-src/gradlew \
    android-src/build.gradle \
    android-src/gradle/wrapper/gradle-wrapper.properties \
    android-src/gradlew.bat \
    android-src/res/values/libs.xml \
    android-src/src/link/basov/asnp/VPlay.java

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android-src

contains(ANDROID_TARGET_ARCH,) {
    ANDROID_ABIS = \
        armeabi-v7a
}

ANDROID_ABIS = armeabi-v7a

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_ABIS = \
        armeabi-v7a
}

