#-------------------------------------------------
#
# Project created by QtCreator 2013-08-07T21:52:15
#
#-------------------------------------------------

QT       += core gui widgets quick

TARGET = Tox-Gui

TEMPLATE = app

CONFIG += c++11

CONFIG += debug_and_release

Release:DESTDIR = build
Release:OBJECTS_DIR = release/obj
Release:MOC_DIR = release/moc
Release:RCC_DIR = release/rcc

Debug:DESTDIR = debug
Debug:OBJECTS_DIR = debug/obj
Debug:MOC_DIR = debug/moc
Debug:RCC_DIR = debug/rcc

INCLUDEPATH += ./submodule/ProjectTox-Core/toxcore/

# Build fails without fpermissive because of code included in tox core.
# Ideally we wouldn't need this flag
QMAKE_CXXFLAGS += -fpermissive

win32:INCLUDEPATH += ../../libs/sodium/include/
macx:INCLUDEPATH += /usr/local/include


win32 {
    LIBS += -lWS2_32 ../../libs/sodium/lib/libsodium.a
} else {
    macx {
        LIBS += -L/usr/local/lib -lsodium
    } else {
        LIBS += -lsodium
    }
}

LIBS += -ltoxcore

SOURCES += main.cpp \
    src/core.cpp \
    src/coremodel.cpp \
    src/settings.cpp \
    src/user.cpp \
    src/group.cpp


HEADERS  += \
    src/core.h \
    src/coremodel.h \
    src/statuswrapper.h \
    src/settings.h \
    src/user.h \
    src/group.h

OTHER_FILES += \
    resources/qml/RequestFriendWindow.qml \
    resources/qml/FriendRequesetWindow.qml \
    resources/qml/ChatArea.qml \
    resources/qml/main.qml

RESOURCES += \
    resources/qml.qrc \
    resources/icons.qrc
