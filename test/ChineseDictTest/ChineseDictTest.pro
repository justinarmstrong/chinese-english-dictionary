include(gtest_dependency.pri)

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG += thread
#CONFIG -= qt

CONFIG += testcase #Creates 'check' target in Makefile.
CONFIG += console

QT += core
CONFIG += c++11
INCLUDEPATH += ../../app/ChineseDictApp

HEADERS +=     tst_pinyinutils.h \
    tst_pinyinutils.h \
    ../../sqlite-amalgamation-3220000/sqlite3.h \
    ../../app/ChineseDictApp/textutils.h

SOURCES +=     main.cpp \
    ../../sqlite-amalgamation-3220000/sqlite3.c \
    ../../app/ChineseDictApp/textutils.cpp
