TEMPLATE = app
TARGET = tst_parser

QT       += testlib
QT       -= gui

CONFIG   += console c++11
CONFIG   -= app_bundle

INCLUDEPATH += ../src/

DEFINES += SRCDIR=\\\"$$PWD/\\\"

SOURCES += tst_parser.cpp \
    ../src/parser.cpp \
    ../src/errors.cpp \

RESOURCES += \
    parser.qrc
