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

#ifndef DICTDB_H
#define DICTDB_H

#include <QObject>
#include <QThread>

#include "qobjectlistmodel.h"
#include "sqlite3.h"


class SearchResult : public QObject
{
    Q_OBJECT

private:

    int m_wordsKey;
    QString m_traditional;
    QString m_simplified;
    QString m_pinyin;
    QString m_toneNums;
    QString m_english;

public:
    explicit SearchResult(int wordsKey,
                       QString traditional,
                       QString simplified,
                       QString pinyin,
                       QString toneNums,
                       QString english)
                        :
        m_wordsKey(wordsKey),
        m_traditional(traditional),
        m_simplified(simplified),
        m_pinyin(pinyin),
        m_toneNums(toneNums),
        m_english(english)
    {

    }

    Q_PROPERTY(int wordsKey READ wordsKey CONSTANT)
    Q_PROPERTY(QString traditional READ traditional CONSTANT)
    Q_PROPERTY(QString simplified READ simplified CONSTANT)
    Q_PROPERTY(QString pinyin READ pinyin CONSTANT)
    Q_PROPERTY(QString toneNums READ toneNums CONSTANT)
    Q_PROPERTY(QString english READ english CONSTANT)

    int wordsKey() { return m_wordsKey; }
    QString traditional() { return m_traditional; }
    QString simplified() { return m_simplified; }
    QString pinyin() { return m_pinyin; }
    QString toneNums() { return m_toneNums; }
    QString english() { return m_english; }


};


class DictDb : public QObject
{
    Q_OBJECT

public:
private:
    sqlite3* db;

    sqlite3_stmt* m_englishQueryStmt;
    sqlite3_stmt* m_englishShortQueryStmt;
    sqlite3_stmt* m_pinyinQueryStmt;
    sqlite3_stmt* m_tonelessPinyinQueryStmt;
    sqlite3_stmt* m_simplifiedQueryStmt;
    sqlite3_stmt* m_traditionalQueryStmt;
    sqlite3_stmt* m_wordsKeyQueryStmt;

    sqlite3_stmt* m_componentQueryStmt;
    sqlite3_stmt* m_classifiersForWordQueryStmt;

    sqlite3_stmt* m_allWordsQueryStmt;

    QThread m_thread;

 private:
    bool openDb();
    void makeStatements();
    bool addWord(sqlite3_stmt *insertStmt,
            QString traditional,
            QString simplified,
            QString displayPinyin,
            QString spacelessPinyin,
            QString tonelessPinyin,
            QString english,
            QString classifier,
            QString alsoWrittenAs);

    void sendExtraInfo
        (int wordsKey,
         const QString& rawEnglish,
         const QString& alsoWritten,
         const QString& alsoPronounced);

    void sendClassifiers(const QString &classifiers);

    void prepareStatement(QString& query, sqlite3_stmt **stmt);

public:

    explicit DictDb();

    explicit DictDb(QObject *parent) :
        QObject(parent)
    {
    }

    ~DictDb() {
        m_thread.quit();
        m_thread.wait();

        sqlite3_finalize(m_englishQueryStmt);
        sqlite3_finalize(m_englishShortQueryStmt);
        sqlite3_finalize(m_pinyinQueryStmt);
        sqlite3_finalize(m_tonelessPinyinQueryStmt);
        sqlite3_finalize(m_simplifiedQueryStmt);
        sqlite3_finalize(m_traditionalQueryStmt);
        sqlite3_finalize(m_wordsKeyQueryStmt);
        sqlite3_finalize(m_componentQueryStmt);
        sqlite3_finalize(m_classifiersForWordQueryStmt);
        sqlite3_close(db);

        //qDebug() << "~DictDb";
    }

    void start();

signals:

    //used to connect to QML in UI thread
    void searchInProgressChanged(QVariant inProgress);
    void extraInfoChanged(QVariant english);
    void classifiersChanged(QVariant english);
    void componentCharactersChanged(QVariant components);

    //used to connect to resultsModel in UI thread
    void clearAndDeleteResultList();
    void changeResultList(QObjectList* objects);


public slots:

    void onMatchEnglishAsync(const QString& search);
    void onMatchChineseAsync(const QString& search);
    void sendComponentCharacters(const QString& characters, const QString &pinyin, const QString &toneNums);

    void onRequestDetailsAsync(int wordsKey);
};

//QML_DECLARE_TYPE(DictDb)

#endif // DICTDB_H
