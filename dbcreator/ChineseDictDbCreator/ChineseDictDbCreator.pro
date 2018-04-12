TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += ../../app/ChineseDictApp

SOURCES += \
    ../../sqlite-amalgamation-3220000/sqlite3.c \
    dbcreator.cpp \
    ../../app/ChineseDictApp/textutils.cpp \
    main.cpp

HEADERS += \
    ../../sqlite-amalgamation-3220000/sqlite3.h \
    dbcreator.h \
    ../../app/ChineseDictApp/textutils.h
