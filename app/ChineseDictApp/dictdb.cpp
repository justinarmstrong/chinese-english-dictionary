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

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QVariant>
#include <QRegularExpression>
#include <QQmlPropertyMap>
#include <QStandardPaths>
#include <QStringList>
#include <QChar>
#include <unistd.h>

#include <assert.h>
#include "dictdb.h"
#include "textutils.h"


DictDb::DictDb()
{
   if (!openDb()) {
       qDebug() << "error, could not find db file";
       exit(1);
   }

   makeStatements();

}

void DictDb::start() {
    m_thread.start();
    moveToThread(&m_thread);
}

bool DictDb::openDb()
{

#ifdef Q_OS_ANDROID
    QFile dbFile("assets:/words.db");
    QString filePath = QStandardPaths::writableLocation( QStandardPaths::StandardLocation::AppLocalDataLocation );
    filePath.append( "/words.db");
    qDebug() << "filePath is" << filePath;
    if (dbFile.exists()) {
        if( QFile::exists( filePath ) )
            QFile::remove( filePath );

        if( dbFile.copy( filePath ) )
            QFile::setPermissions( filePath, QFile::WriteOwner | QFile::ReadOwner );
    }
#else
    QString dbDirectory = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);  //TODO
    QString filePath = dbDirectory + "/words.db";
#endif
    int ret = sqlite3_open_v2(filePath.toUtf8(), &db, SQLITE_OPEN_READONLY, NULL);
    if ((ret == SQLITE_OK) && (db != NULL)) {
        qDebug() << "words db exists, opened ok";
        return true;
    }
    qDebug() << "Error opening " << filePath;
    return false;
}

void DictDb::prepareStatement(QString& query, sqlite3_stmt** stmt)
{
    int ret;
    //qDebug() << "prepareStatement: " << query;
    QByteArray queryUtf8 = query.toUtf8();
    ret = sqlite3_prepare_v2(db,
        queryUtf8.constData(),
        queryUtf8.size(),
        stmt,
        NULL);

    if (ret != SQLITE_OK) {
        qDebug() << "Error preparing statement <" << query << "> ";
        qDebug() << "error: " << sqlite3_errmsg(db);
      assert(false);
    }
    assert(stmt);
}

void DictDb::makeStatements()
{
    QString query;

    //m_simplifiedQueryStmt
    query =
            "SELECT rowid,* "
            "FROM words WHERE "
            "simplified MATCH ? "
             "ORDER BY word_rank ASC ";
    prepareStatement(query, &m_simplifiedQueryStmt);

    //m_traditionalQueryStmt
    query =
            "SELECT rowid,* "
            "FROM words WHERE "
            "traditional MATCH ? "
            "ORDER BY word_rank ASC ";
    prepareStatement(query, &m_traditionalQueryStmt);


    //_pinyinQueryStmt: tonemarked pinyin
    query =
            "SELECT rowid,* "
            "FROM words WHERE "
            "pinyin_spaceless MATCH ? "
            "ORDER BY word_rank ASC ";
    prepareStatement(query, &m_pinyinQueryStmt);

    //_tonelessPinyinQueryStmt: toneless pinyin
    query =
            "SELECT rowid,* "
            "FROM words WHERE "
            "pinyin_toneless MATCH ? "
            "ORDER BY word_rank ASC ";
    prepareStatement(query, &m_tonelessPinyinQueryStmt);

    //_englishQueryStmt
    query =
            "SELECT rowid,* "
            "FROM words WHERE english MATCH ? "
            "ORDER BY word_rank ASC ";
    prepareStatement(query, &m_englishQueryStmt);

    //FIXME worth bothering with??
    //_englishShortQueryStmt: used when there are likely to be an enormous number of
    //useless results
    query =
            "SELECT rowid,* "
            "FROM words WHERE english MATCH ? "
            "ORDER BY word_rank ASC "
            "LIMIT 50;";
    prepareStatement(query, &m_englishShortQueryStmt);


    //m_classifiersForWordQueryStmt: used to look up all words that use a classifier
    query =
        "SELECT rowid,* "
        "FROM words WHERE classifiers MATCH ? "
        "ORDER BY word_rank ASC";
    prepareStatement(query, &m_classifiersForWordQueryStmt);

    //m_wordsKeyQueryStmt: used when we already have the key
    query =
            "SELECT rowid,* "
            "FROM words WHERE rowid = ?;";
    prepareStatement(query, &m_wordsKeyQueryStmt);

    //m_componentQueryStmt: used in details page to look up individual characters in a definition
    query =
        "SELECT english, simplified, pinyin, pinyin_toneless "
        "FROM words WHERE traditional MATCH ? "
        "ORDER BY word_rank ASC";
    prepareStatement(query, &m_componentQueryStmt);

    //m_allWordsQueryStmt;
    query =
        "SELECT rowid,* "
        "FROM words WHERE traditional != simplified "
        "ORDER BY word_rank ASC;";
        //"LIMIT 1000;";
    prepareStatement(query, &m_allWordsQueryStmt);

}

static void AppendSearchResultRow(QObjectList* results, sqlite3_stmt* stmt)
{
    int rowid = sqlite3_column_int(stmt, 0);
    QString traditional = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 1));
    QString simplified = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 2));
    QString pinyin = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 3));
    QString spaceless = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 4));
    QString toneless = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 5));
    QString english = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 6));
    QString toneNums = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 10));
    int wordRank = sqlite3_column_int(stmt, 12);

    //if (traditional.length() > 1) return;
    SearchResult* result = new SearchResult(rowid, traditional,
                                      simplified, pinyin, toneNums,
                                      english);
    results->append(result);
/*
    qDebug() << wordRank
         << " " << traditional
         << " " << simplified
         << " " << pinyin
         << " " << spaceless
         << " " << toneless
         << " " << english
         << " " << toneNums;
*/
}

void DictDb::onMatchChineseAsync(const QString& search)
{

    //we need to cope with
    //1  simplified hanzi
    //2. traditional hanzi
    //3.1 'toneless' pinyin (i.e. no tonemarks or numbers)
    //3.2 pinyin with numbers
    //3.3 pinyin with tonemarks
    //4. 'CL:' special mode to search by classifier
    qDebug() << "searching for " << search;

    sqlite3_stmt* stmt = NULL;
    int ret;
    emit clearAndDeleteResultList();

    if (search.length() == 0) return;

    emit searchInProgressChanged(true);

    QObjectList* results = new QObjectList;

    //sleep(1); //to test delays

    textFormat_t textFormat = determineTextFormat(search);

    if (textFormat == tfHanzi) {
        if (search.startsWith("CL:")) {
            //SPECIAL MODE - search for classifers!
            QString query = search + "*";
            query.remove("CL:");
            //qDebug() << "searching for classifiers... " << query;
            stmt = m_classifiersForWordQueryStmt;
            sqlite3_bind_text(stmt, 1, query.toUtf8(), -1, SQLITE_TRANSIENT);
            while((ret = sqlite3_step(stmt)) == SQLITE_ROW) {
                AppendSearchResultRow(results, stmt);
            }
        } else {
            //regular character search
            //qDebug() << "seems to be characters...";
            QString query = search + "*";
            //try simplified
            stmt = m_simplifiedQueryStmt;
            sqlite3_bind_text(stmt, 1, query.toUtf8(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, query.toUtf8(), -1, SQLITE_TRANSIENT);
            while((ret = sqlite3_step(stmt)) == SQLITE_ROW) {
                AppendSearchResultRow(results, stmt);
            }
            if (results->count() == 0) {
                //qDebug() << " no matches, trying traditional " << query;
                sqlite3_reset(stmt);
                stmt = m_traditionalQueryStmt;
                sqlite3_bind_text(stmt, 1, query.toUtf8(), -1, SQLITE_TRANSIENT);
                while((ret = sqlite3_step(stmt)) == SQLITE_ROW) {
                    AppendSearchResultRow(results, stmt);
                }
            }
        }

    } else {
        //some form of pinyin

        //remove spaces and make lowercase
        QString query = search.toLower();
        query.remove(' ');

        if (textFormat == tfPinyinNoTones) {
            //remove umaluts, the toneless column treats them as "u"
            //query.replace(QString::fromWCharArray(L"ü"), QChar('u'));
            //query.replace(QString("u:"), QChar('u'));
            //query.replace(QString("v"), QChar('u'));

            //qDebug() <<  "toneless pinyin";
            stmt = m_tonelessPinyinQueryStmt;
            query = makeTonelessSearchPinyin(query);
        } else {
            stmt = m_pinyinQueryStmt;
            if (textFormat == tfPinyinNumbers) {
                query = makeToneMarkedSearchPinyin(query);
                //qDebug() << " numbered pinyin, converted to " << query;
            } // else already tonemarked pinyin
        }
        sqlite3_bind_text(stmt, 1, query.toUtf8(), -1, SQLITE_TRANSIENT);

        while((ret = sqlite3_step(stmt)) == SQLITE_ROW) {
            AppendSearchResultRow(results, stmt);
        }
    }

    qDebug() << "got " << results->count() << " results";

    sqlite3_reset(stmt);

    emit changeResultList(results);

    emit searchInProgressChanged(false);
}


void DictDb::onMatchEnglishAsync(const QString& search)
{
    sqlite3_stmt* stmt;

    emit clearAndDeleteResultList();
/*
    stmt = m_allWordsQueryStmt;
    sqlite3_bind_text(stmt, 1, search.toUtf8(), -1, SQLITE_TRANSIENT);
    emit searchInProgressChanged(true);
    QObjectList* results = new QObjectList;
    int ret;
    while((ret = sqlite3_step(stmt)) == SQLITE_ROW) {
        AppendSearchResultRow(results, stmt);
    }

    sqlite3_reset(stmt);
    emit changeResultList(results);

    emit searchInProgressChanged(false);
*/
    if (search.length() == 0) return;

    emit searchInProgressChanged(true);

    //if the user types "a" they will get thousands of
    //results which are probably not useful and will
    //make it run very slowly.
    //so we limit the number of results for less than 3 letters

    if (search.length() < 3) {
        stmt = m_englishShortQueryStmt;
        //qDebug() << " using limited query for " << search;
    } else {
        //qDebug() << " using long query for " << search;
        stmt = m_englishQueryStmt;
    }

    sqlite3_bind_text(stmt, 1, search.toUtf8(), -1, SQLITE_TRANSIENT);


    //QTime myTimer;
    //myTimer.start();

    QObjectList* results = new QObjectList;
    int ret;
    while((ret = sqlite3_step(stmt)) == SQLITE_ROW) {
        AppendSearchResultRow(results, stmt);
    }

    sqlite3_reset(stmt);

    //int nMilliseconds = myTimer.elapsed();
    //qDebug() << "got " << results->count() << " results in " << (float)(nMilliseconds)/1000 << "secs";

    emit changeResultList(results);

    emit searchInProgressChanged(false);

}

void DictDb::sendExtraInfo
    (int wordsKey,
     const QString& rawEnglish,
     const QString& alsoWritten,
     const QString& alsoPronounced)
{
    //QTime myTimer;
    //myTimer.start();
    QString englishJson = "[";
    QStringList definitions = rawEnglish.split("/");
    int i;
    for (i=0; i < definitions.count(); i++) {
        QString def = definitions[i];
        def.replace("\"", "\\\"");
        if (i > 0) englishJson += ",";
        englishJson += "{ \"english\": \"" + def + "\", \"inlineChinese\": [";
        QRegExp hanziPairMatch("(([\\x4e00-\\x9fa5]+)|([\\x4e00-\\x9fa5]+)\\|([\\x4e00-\\x9fa5]+))");
        int pos = 0;
        int foundHanzi = 0;
        while ((pos = hanziPairMatch.indexIn(def, pos)) != -1) {
            //qDebug() << " " << hanziPairMatch.cap(1);
            if (foundHanzi > 0) englishJson += ",";
            englishJson += "\"" + hanziPairMatch.cap(1) + "\"";
            foundHanzi++;
            pos += hanziPairMatch.matchedLength();
        }
        englishJson += "]}";

    }
    englishJson += "]";
    //qDebug() << "englishJson = " << englishJson;

    QString chineseJson = "{\n";
    chineseJson += " \"alsoWritten\": \"" + alsoWritten + "\", \n";
    chineseJson += " \"alsoPronounced\": \"" + alsoPronounced + "\"\n";
    chineseJson += "}";
    //qDebug() << "chineseJson = " << chineseJson;

    QString extraInfoJson = "{";
    extraInfoJson += "\"wordsKey\": " + QString::number(wordsKey) + ",\n";
    extraInfoJson += "\"english\": " + englishJson + ",\n";
    extraInfoJson += "\"chinese\": " + chineseJson + "\n";
    extraInfoJson += "}";
    //qDebug() << "extraInfoJson = " << extraInfoJson;
    emit extraInfoChanged(extraInfoJson);

    //int nMilliseconds = myTimer.elapsed();
    //qDebug() << "sendBasicDefs took " << (float)nMilliseconds << "secs";
}


void DictDb::sendClassifiers(const QString& classifiers)
{
    QString classifiersJson = "[";
    QStringList classifierList = classifiers.split(",");
    int i;
    uint classifierCount = 0;
    for (i=0; i < classifierList.count(); i++) {
        QString classifierCharacter = classifierList[i];

        //now we need to look up this character. it is always a trad character
        sqlite3_bind_text(m_traditionalQueryStmt, 1, classifierCharacter.toUtf8(), -1, SQLITE_TRANSIENT);
        QString english;
        QString simplified;    //we pick these up now so we can provide them to the ui
        QString pinyin;
        QString toneNum;
        int wordRet;
        do {
            wordRet = sqlite3_step(m_traditionalQueryStmt);
            if (wordRet == SQLITE_ROW) {
                simplified = QString::fromUtf8((const char*)sqlite3_column_text(m_traditionalQueryStmt, 2));
                pinyin = QString::fromUtf8((const char*)sqlite3_column_text(m_traditionalQueryStmt, 3));
                english = QString::fromUtf8((const char*)sqlite3_column_text(m_traditionalQueryStmt, 6));
                toneNum = QString::fromUtf8((const char*)sqlite3_column_text(m_traditionalQueryStmt, 10));
                //qDebug() << " classifier english got " << english;
            }
        } while ((wordRet == SQLITE_ROW) &&
                 (!english.contains("classifier for")));

        if (!english.isEmpty()) {
            //the english text should contain 1 or more classifier
            // split it into separate defs, and extract all the classifiers

            if (classifierCount > 0) classifiersJson += ", ";
            classifiersJson += "{ ";
            classifiersJson += "\"traditional\": \"" + classifierCharacter + "\", ";
            classifiersJson += "\"simplified\": \"" + simplified + "\", ";
            classifiersJson += "\"pinyin\": \"" + pinyin + "\", ";
            classifiersJson += "\"toneNums\": \"" + toneNum + "\", ";
            classifiersJson += "\"english\": \"";
            QStringList definitions = english.split("/");

            int i;
            uint defCount = 0;
            for (i=0; i < definitions.count(); i++) {
                QString def = definitions[i];
                if (def.startsWith("classifier")) {
                    if (defCount > 0) classifiersJson += ";  ";
                    classifiersJson += def;
                    defCount++;
                }
            }

            if (defCount == 0) {
                //no proper "classifier for" definition
                //so just use the full english text
                classifiersJson += english;
            }

            classifiersJson += "\" }";
            classifierCount++;
        } //english
        sqlite3_reset(m_traditionalQueryStmt);
    }
    classifiersJson += "]";
    //qDebug() << "dictDb: classifiersJson is " << classifiersJson;

    emit classifiersChanged(classifiersJson);
}

//if there are several matches we use a heuristic to try and get the right one
//we keep looking as long as either the pinyin doesn't match or
//the english definition says it is a surname or abbreviation for something
bool static isReasonableMatch
    (const QString& candidateEnglish,
     const QString& candidatePinyin,
     const QString& candidateTonelessPinyin,
     const QString& pinyin)
{
    if ((candidateEnglish.startsWith("surname")) || (candidateEnglish.contains("abbr."))) return false;

    //qDebug() << " candidatePinyin: " << candidatePinyin << " candidateTonelessPinyin: " << candidateTonelessPinyin;
    if ((pinyin != candidatePinyin) && (pinyin != candidateTonelessPinyin)) return false;
    return true;
}

void DictDb::sendComponentCharacters(const QString& characters, const QString& pinyin, const QString& toneNums)
{
    //some characters have multiple meanings
    //so we use the pinyin as an additional qualifier

    QStringList pinyinWordList = pinyin.split(',');
    QStringList toneNumList = toneNums.split(',');

    int charCount = characters.size();
    qDebug() << pinyin << pinyinWordList << toneNumList << characters;
    /*
    if (pinyinWordList.count() != charCount) {
        qDebug() << "Error, pinyin has " << pinyinWordList.count() << " characters, charCount has " << charCount;
        return;
    }
*/
    //qDebug() << pinyinWordList << toneNumList;

    QString componentsJson = "[";

    int ret;
    sqlite3_stmt* stmt = m_componentQueryStmt;

    int charIndex;
    int actualHanziIndex = 0;
    for (charIndex=0; charIndex < charCount; charIndex++) {
        //QTime myTimer;
        //myTimer.start();
        QString currentChar = characters.at(charIndex);
        if (isPunctuation(currentChar)) {
            //qDebug() << "skipping" << currentChar;
            continue; //skip char and pinyin syllable
        }

        if (actualHanziIndex >= toneNumList.count()) {
            qDebug() << "Error, toneNumList has " << toneNumList.count() << " characters, toneNumIndex " << actualHanziIndex;
            break;
        }

        QString simplified;
        QString matchEnglish;
        QString matchPinyin;
        QString matchTonelessPinyin;
        QString targetPinyin;
        QString targetToneNum = "5";

        if (isHanzi(currentChar)) {
            targetPinyin = pinyinWordList.at(actualHanziIndex);
            targetToneNum = toneNumList.at(actualHanziIndex);

            //qDebug() << "checking... currentChar: " << currentChar << " pinyin: " << targetPinyin;
            sqlite3_bind_text(stmt, 1, currentChar.toUtf8(), -1, SQLITE_TRANSIENT);

            do {
                ret = sqlite3_step(stmt);
                if (ret == SQLITE_ROW) {
                    simplified = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 1));
                    matchEnglish = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 0));
                    matchPinyin = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 2));
                    matchTonelessPinyin = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 3));
                    //qDebug() << " match - english: " << english << " pinyin: " << matchPinyin << " currentChar: " << currentChar;
                }

            } while ((ret == SQLITE_ROW) && (!isReasonableMatch(matchEnglish, matchPinyin, matchTonelessPinyin, targetPinyin)));

            sqlite3_reset(stmt);
            //int nMilliseconds = myTimer.elapsed();
            //qDebug() << "took " << (float)(nMilliseconds)/1000 << "secs";
        }

        //this would only occur for very rare situation where the character didn't exist on its own in the db
        //e.g. fake characters like "Ｕ" in "ＵＳＢ手指"
        if (simplified.isEmpty()) simplified = currentChar;
        matchEnglish.replace("\"", "\\\"");
        if (actualHanziIndex > 0) componentsJson +=",";
        componentsJson += "{ ";
        componentsJson += "\"traditional\": \"" + currentChar + "\", ";
        componentsJson += "\"simplified\": \"" + simplified + "\", ";
        componentsJson += "\"pinyin\": \"" + targetPinyin + "\", ";
        componentsJson += "\"toneNums\": \"" + targetToneNum + "\", ";
        componentsJson += "\"english\": \"" + matchEnglish + "\"";
        componentsJson += "}";

        actualHanziIndex++;
    }
    componentsJson += "]";
    //qDebug() << "componentsJson:" << componentsJson;
    emit componentCharactersChanged(componentsJson);

}

void DictDb::onRequestDetailsAsync(int wordsKey)
{
    //qDebug() << "looking for " << wordsKey;

    sqlite3_stmt* stmt = m_wordsKeyQueryStmt;
    sqlite3_bind_int(stmt, 1, wordsKey);
    int ret = sqlite3_step(stmt);
    if (ret != SQLITE_ROW) {
        qDebug() << "onRequestDetailsAsync error, got " << ret;
        return;
    }
    //qDebug() << " got " << ret;
    QString traditional = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 1));
    //QString simplified = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 2));
    //QString pinyin = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 4));
    QString english = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 6));
    QString alsoWritten = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 7));
    QString alsoPronounced = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 8));
    QString classifiers = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 9));
    QString toneNums = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 10));
    QString componentPinyin = QString::fromUtf8((const char*)sqlite3_column_text(stmt, 11));
    sqlite3_reset(stmt);

    //qDebug() << "classifiers:" << classifiers;

    sendExtraInfo(wordsKey,
                  english,
                  alsoWritten,
                  alsoPronounced);

    sendClassifiers(classifiers);
    sendComponentCharacters(traditional, componentPinyin, toneNums);
}



