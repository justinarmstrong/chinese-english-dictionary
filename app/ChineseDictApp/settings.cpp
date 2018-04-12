/*
 * Copyright Justin Armstrong 2012, 2018.
 *
 * This file is part of the application "Chinese-English Dictionary for Qt"
 *
 * "Chinese-English Dictionary for Qt" is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <assert.h>
#include <sys/stat.h>

#include "settings.h"
#include "dictdb.h"

#define DB_NAME ".chinesedict.settings.1.db"

void Settings::prepareStatement(QString& query, sqlite3_stmt** stmt)
{
    int ret;
    QByteArray queryUtf8 = query.toUtf8();
    ret = sqlite3_prepare_v2(m_prefsDb,
        queryUtf8.constData(),
        queryUtf8.size(),
        stmt,
        NULL);

    if (ret != SQLITE_OK) {
        qDebug() << "Error preparing statement <" << query << "> ";
        qDebug() << "error: " << sqlite3_errmsg(m_prefsDb);
      assert(false);
    }
    assert(stmt);
}

Settings::Settings()
{
    QString settingsDirectory = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    //QString settingsDirectory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);  //TODO
    QString settingsFilePath = settingsDirectory + "/" + DB_NAME;
    qDebug() << "x settings db path: " << settingsFilePath;
    if (!QFile(settingsFilePath).exists()) {
        qDebug() << "creating db";
        if (!QDir().mkpath(settingsDirectory)) {
            qDebug() << "error creating directory, exiting";
            exit(1);
        }
        if (!createDb(settingsFilePath)) {
            qDebug() << "error creating db, exiting";
            exit(1);
        }
        chmod(settingsFilePath.toLatin1(), S_IRUSR|S_IWUSR|S_IROTH|S_IWOTH);
    }
    int ret = sqlite3_open_v2(settingsFilePath.toLatin1(), &m_prefsDb, SQLITE_OPEN_READWRITE, NULL);
    if ((ret == SQLITE_OK) && (m_prefsDb != NULL)) {
        qDebug() << " opened" << settingsFilePath;
    }

    QString query;

    query = "SELECT * FROM preferences";
    prepareStatement(query, &m_getPreferencesStmt);

    query = "SELECT * FROM favourites WHERE words_key=?";
    prepareStatement(query, &m_getFavouriteStmt);

    query = "INSERT INTO favourites VALUES (?, ?, ?, ?, ?, ?)";
    prepareStatement(query, &m_addFavouriteStmt);

    query = "DELETE FROM favourites WHERE words_key=?";
    prepareStatement(query, &m_removeFavouriteStmt);

    query = "SELECT * FROM favourites";
    prepareStatement(query, &m_getAllFavouritesStmt);

    loadPreferences();
    loadFavourites();
}

bool Settings::createDb(const QString &filePath)
{
    //no db, try to create one
    int ret = sqlite3_open_v2(filePath.toLatin1(), &m_prefsDb, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if ((ret != SQLITE_OK) || (m_prefsDb == NULL)) {
        qDebug() << "Error creating settings database: " << ret;
        return false;
     }

    char *errmsg;
    ret = sqlite3_exec(m_prefsDb,
        "CREATE TABLE favourites ( "
               "words_key integer,"
               "traditional text,"
               "simplified text,"
               "pinyin text,"
               "toneNums text,"
               "english text);",
         NULL, 0, &errmsg);

    if(ret != SQLITE_OK) {
        qDebug() << "Error creating favourites table: " << errmsg;
      return false;
    }
    qDebug() << "created favourites table ok";

    ret = sqlite3_exec(m_prefsDb,
        "CREATE TABLE preferences ( "
               "search_by_chinese integer,"
               "use_traditional integer,"
               "use_tone_colours integer,"
               "tone1_colour text,"
               "tone2_colour text,"
               "tone3_colour text,"
               "tone4_colour text,"
               "tone5_colour text,"
               "spare1 integer);",
         NULL, 0, &errmsg);

    if(ret != SQLITE_OK) {
        qDebug() << "Error creating preferences table: " << errmsg;
      return false;
    }

    ret = sqlite3_exec(m_prefsDb,
        "INSERT INTO preferences VALUES (0, 0, 0, '#ff0000', '#ff8a21', '#218c17', '#1764cb', '#323332', 0);",
         NULL, 0, &errmsg);

    if(ret != SQLITE_OK) {
        qDebug() << "Error inserting default values into preferences table: " << errmsg;
      return false;
    }

    qDebug() << "created preferences table ok";

    return true;
}

void Settings::loadPreferences()
{
    sqlite3_stmt* stmt = m_getPreferencesStmt;
    int ret = sqlite3_step(stmt);
    if ((ret != SQLITE_ROW) && (ret != SQLITE_DONE)) {
        qDebug() << "Error reading from preferences table " << ret;
    }
    m_searchByChinese = sqlite3_column_int(stmt, 0);
    m_useTraditional = sqlite3_column_int(stmt, 1);
    m_toneColorsEnabled = sqlite3_column_int(stmt, 2);
    m_tone1Color = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 3));
    m_tone2Color = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 4));
    m_tone3Color = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 5));
    m_tone4Color = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 6));
    m_tone5Color = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 7));

    qDebug() << "searchByChinese " << m_searchByChinese;
    qDebug() << "useTraditional " << m_useTraditional;
    qDebug() << "m_tone1Color " << m_tone1Color;
    qDebug() << "m_tone2Color " << m_tone2Color;
    qDebug() << "m_tone3Color " << m_tone3Color;
    qDebug() << "m_tone4Color " << m_tone4Color;
    qDebug() << "m_tone5Color " << m_tone5Color;

    sqlite3_reset(stmt);
}

void Settings::loadFavourites()
{
    QObjectList* results = new QObjectList;
    sqlite3_stmt* stmt = m_getAllFavouritesStmt;
    int ret;
    while((ret = sqlite3_step(stmt)) == SQLITE_ROW) {
        int rowid = sqlite3_column_int(stmt, 0);
        QString traditional = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 1));
        QString simplified = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 2));
        QString pinyin = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 3));
        QString toneNums = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 4));
        QString english = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 5));

        SearchResult* result = new SearchResult(rowid, traditional,
                                          simplified, pinyin, toneNums,
                                          english);
        results->append(result);
        //qDebug() << " added " << simplified;
    }
    sqlite3_reset(stmt);
    m_favouritesList.clearAndDelete();
    m_favouritesList.setObjectList(results);
    emit favouritesListChanged();
}

void Settings::addFavourite(int key,
                           QString traditional,
                           QString simplified,
                           QString pinyin,
                           QString toneNums,
                           QString english)
{


    sqlite3_stmt* stmt = m_addFavouriteStmt;
    sqlite3_bind_int(stmt, 1, key);
    sqlite3_bind_text(stmt, 2, traditional.toUtf8(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, simplified.toUtf8(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, pinyin.toUtf8(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, toneNums.toUtf8(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, english.toUtf8(), -1, SQLITE_TRANSIENT);

    int ret = sqlite3_step(stmt);
    sqlite3_reset(stmt);
    //qDebug() << "addFavourite " << ret;
    if(ret != SQLITE_DONE) {
        qDebug() << "Error inserting into favourites table ";
    }
    loadFavourites();
}

void Settings::removeFavourite(int key, int listRowIndex)
{
    sqlite3_stmt* stmt = m_removeFavouriteStmt;
    sqlite3_bind_int(stmt, 1, key);
    int ret = sqlite3_step(stmt);
    sqlite3_reset(stmt);
    //qDebug() << "removeFavourite " << ret;
    if(ret != SQLITE_DONE) {
        qDebug() << "Error removing from favourites table ";
    }
    m_favouritesList.removeAt(listRowIndex);
    emit favouritesListChanged();
    //loadFavourites();
}

bool Settings::isFavourite(int key)
{

    sqlite3_stmt* stmt = m_getFavouriteStmt;
    sqlite3_bind_int(stmt, 1, key);
    int ret = sqlite3_step(stmt);
    sqlite3_reset(stmt);
    if (ret == SQLITE_ROW) {
        return true;
    }
    return false;
}

void Settings::setSearchByChinese(bool flag)
{    
    QString query = "UPDATE preferences SET search_by_chinese=" + QString::number(flag) + ";";
    char* errmsg;
    int ret = sqlite3_exec(m_prefsDb,
        query.toUtf8(),
        NULL, 0, &errmsg);
    if(ret != SQLITE_OK) {
        qDebug() << "Error setting search_by_chinese flag: " << errmsg;
    }

    m_searchByChinese = flag;
    emit searchByChineseChanged();
}

bool Settings::searchByChinese()
{
    return m_searchByChinese;
}

void Settings::setUseTraditional(bool flag)
{    
    //qDebug() << "setUseTraditional" << flag;
    QString query = "UPDATE preferences SET use_traditional=" + QString::number(flag) + ";";
    char* errmsg;
    int ret = sqlite3_exec(m_prefsDb,
        query.toUtf8(),
        NULL, 0, &errmsg);
    if(ret != SQLITE_OK) {
        qDebug() << "Error setting use_traditional flag: " << errmsg;
    }

    m_useTraditional = flag;
    emit useTraditionalChanged();
}

bool Settings::useTraditional()
{
    return m_useTraditional;
}

void Settings::setToneColorsEnabled(bool flag)
{

    QString query = "UPDATE preferences SET use_tone_colours=" + QString::number(flag) + ";";
    char* errmsg;
    int ret = sqlite3_exec(m_prefsDb,
        query.toUtf8(),
        NULL, 0, &errmsg);

    if(ret != SQLITE_OK) {
        qDebug() << "Error setting use_tone_colours flag: " << errmsg;
    }

    m_toneColorsEnabled = flag;
    emit toneColorsEnabledChanged();
}

bool Settings::toneColorsEnabled()
{
    return m_toneColorsEnabled;
}

void Settings::setTone1Color(const QString &c) {
    if (c != m_tone1Color) {
        QString query = "UPDATE preferences SET tone1_colour='" + c + "';";
        char* errmsg;
        int ret = sqlite3_exec(m_prefsDb,
            query.toUtf8(),
            NULL, 0, &errmsg);
        if(ret != SQLITE_OK) {
            qDebug() << "Error setting tone1_colour flag: " << errmsg;
        }

        m_tone1Color = c;
        emit tone1ColorChanged();
    }
}

QString Settings::tone1Color() {
    return m_tone1Color;
}

void Settings::setTone2Color(const QString &c) {
    if (c != m_tone2Color) {
        QString query = "UPDATE preferences SET tone2_colour='" + c + "';";
        char* errmsg;
        int ret = sqlite3_exec(m_prefsDb,
            query.toUtf8(),
            NULL, 0, &errmsg);
        if(ret != SQLITE_OK) {
            qDebug() << "Error setting tone2_colour flag: " << errmsg;
        }

        m_tone2Color = c;
        emit tone2ColorChanged();
    }
}

QString Settings::tone2Color() {
    return m_tone2Color;
}

void Settings::setTone3Color(const QString &c) {
    if (c != m_tone3Color) {
        QString query = "UPDATE preferences SET tone3_colour='" + c + "';";
        char* errmsg;
        int ret = sqlite3_exec(m_prefsDb,
            query.toUtf8(),
            NULL, 0, &errmsg);
        if(ret != SQLITE_OK) {
            qDebug() << "Error setting tone3_colour flag: " << errmsg;
        }

        m_tone3Color = c;
        emit tone3ColorChanged();
    }
}

QString Settings::tone3Color() {
    return m_tone3Color;
}

void Settings::setTone4Color(const QString &c) {
    if (c != m_tone4Color) {
        QString query = "UPDATE preferences SET tone4_colour='" + c + "';";
        char* errmsg;
        int ret = sqlite3_exec(m_prefsDb,
            query.toUtf8(),
            NULL, 0, &errmsg);
        if(ret != SQLITE_OK) {
            qDebug() << "Error setting tone4_colour flag: " << errmsg;
        }

        m_tone4Color = c;
        emit tone4ColorChanged();
    }
}

QString Settings::tone4Color() {
    return m_tone4Color;
}

void Settings::setTone5Color(const QString &c) {
    if (c != m_tone5Color) {
        QString query = "UPDATE preferences SET tone5_colour='" + c + "';";
        char* errmsg;
        int ret = sqlite3_exec(m_prefsDb,
            query.toUtf8(),
            NULL, 0, &errmsg);
        if(ret != SQLITE_OK) {
            qDebug() << "Error setting tone5_colour flag: " << errmsg;
        }

        m_tone5Color = c;
        emit tone5ColorChanged();
    }
}

QString Settings::tone5Color() {
    return m_tone5Color;
}
