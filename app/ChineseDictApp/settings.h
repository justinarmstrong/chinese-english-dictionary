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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QQmlContext>
#include <QString>
#include <QQmlEngine>

#include "qobjectlistmodel.h"
#include "sqlite3.h"

class Settings : public QObject
{
    Q_OBJECT
private:
    sqlite3* m_prefsDb;

    sqlite3_stmt* m_getPreferencesStmt;

    sqlite3_stmt* m_getFavouriteStmt;
    sqlite3_stmt* m_addFavouriteStmt;
    sqlite3_stmt* m_removeFavouriteStmt;
    sqlite3_stmt* m_getAllFavouritesStmt;

    QObjectListModel m_favouritesList;

    bool m_useTraditional;
    bool m_searchByChinese;

    bool m_toneColorsEnabled;
    QString m_tone1Color;
    QString m_tone2Color;
    QString m_tone3Color;
    QString m_tone4Color;
    QString m_tone5Color;

    void prepareStatement(QString &query, sqlite3_stmt **stmt);

public:

    explicit Settings();
    explicit Settings(QObject *parent) :
        QObject(parent)
    {
    }

    ~Settings() {
        sqlite3_finalize(m_getPreferencesStmt);
        sqlite3_finalize(m_getFavouriteStmt);
        sqlite3_finalize(m_addFavouriteStmt);
        sqlite3_finalize(m_removeFavouriteStmt);
        sqlite3_finalize(m_getAllFavouritesStmt);
        sqlite3_close(m_prefsDb);

       //qDebug() << "~Settings";
    }


    bool createDb(const QString &filePath);
    void loadPreferences();
    void loadFavourites();

    Q_PROPERTY(bool searchByChinese READ searchByChinese WRITE setSearchByChinese NOTIFY searchByChineseChanged)
    void setSearchByChinese(bool flag);
    bool searchByChinese();

    Q_PROPERTY(bool useTraditional READ useTraditional WRITE setUseTraditional NOTIFY useTraditionalChanged)
    void setUseTraditional(bool flag);
    bool useTraditional();
    
    Q_PROPERTY(bool toneColorsEnabled READ toneColorsEnabled WRITE setToneColorsEnabled NOTIFY toneColorsEnabledChanged)
    void setToneColorsEnabled(bool flag);
    bool toneColorsEnabled();

    Q_PROPERTY(QString tone1Color
               READ tone1Color
               WRITE setTone1Color
               NOTIFY tone1ColorChanged)
    QString tone1Color();
    void setTone1Color(const QString &c);

    Q_PROPERTY(QString tone2Color
               READ tone2Color
               WRITE setTone2Color
               NOTIFY tone2ColorChanged)
    QString tone2Color();
    void setTone2Color(const QString &c);

    Q_PROPERTY(QString tone3Color
               READ tone3Color
               WRITE setTone3Color
               NOTIFY tone3ColorChanged)
    QString tone3Color();
    void setTone3Color(const QString &c);

    Q_PROPERTY(QString tone4Color
               READ tone4Color
               WRITE setTone4Color
               NOTIFY tone4ColorChanged)
    QString tone4Color();
    void setTone4Color(const QString &c);

    Q_PROPERTY(QString tone5Color
               READ tone5Color
               WRITE setTone5Color
               NOTIFY tone5ColorChanged)
    QString tone5Color();
    void setTone5Color(const QString &c);

    Q_PROPERTY(QObjectListModel* favouritesList READ favouritesList NOTIFY favouritesListChanged)
    QObjectListModel* favouritesList() { return &m_favouritesList; }

    Q_INVOKABLE bool isFavourite(int key);
    Q_INVOKABLE void addFavourite(int key, QString traditional, QString simplified, QString pinyin, QString toneNums, QString english);
    Q_INVOKABLE void removeFavourite(int key, int listRowIndex);


signals:
    void favouritesListChanged();
    void searchByChineseChanged();
    void useTraditionalChanged();
    void toneColorsEnabledChanged();
    void tone1ColorChanged();
    void tone2ColorChanged();
    void tone3ColorChanged();
    void tone4ColorChanged();
    void tone5ColorChanged();

public slots:
    
};
QML_DECLARE_TYPE(Settings)

#endif // SETTINGS_H
