#include <QString>
#include <QStringList>
#include <QDebug>
#include <QRegExp>
#include <QFile>
#include <QHash>
#include <assert.h>

#include "dbcreator.h"
#include "textutils.h"
#include "sqlite3.h"


static sqlite3* s_db;
static sqlite3_stmt* s_addWordStmt;
static sqlite3_stmt* s_addClassifierUseStmt;


static bool addWord
(QString traditional,
 QString simplified,
 QString displayPinyin,
 QString spacelessPinyin,
 QString tonelessPinyin,
 QString english,
 QString alsoWrittenAs,
 QString alsoPronounced,
 QString classifiers,
 QString toneNums,
 QString componentPinyin,
 uint wordRank)
{
    //add word to words table
    sqlite3_bind_text(s_addWordStmt, 1, traditional.toUtf8(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(s_addWordStmt, 2, simplified.toUtf8(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(s_addWordStmt, 3, displayPinyin.toUtf8(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(s_addWordStmt, 4, spacelessPinyin.toUtf8(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(s_addWordStmt, 5, tonelessPinyin.toUtf8(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(s_addWordStmt, 6, english.toUtf8(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(s_addWordStmt, 7, alsoWrittenAs.toUtf8(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(s_addWordStmt, 8, alsoPronounced.toUtf8(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(s_addWordStmt, 9, classifiers.toUtf8(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(s_addWordStmt, 10, toneNums.toUtf8(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(s_addWordStmt, 11, componentPinyin.toUtf8(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(s_addWordStmt, 12, wordRank);

    int ret = sqlite3_step(s_addWordStmt);
    if (ret != SQLITE_DONE) {
        qDebug() << "Error inserting :" << sqlite3_errmsg(s_db);
        exit(1);
        return false;
    }
    sqlite3_reset(s_addWordStmt);


    return true;
}

static bool parseFreqListFile(QHash<QString, int>& rankDict, const QString& path)
{
    QFile file(path);

    if (!file.open (QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open:" << file.fileName();
        return false;
    }

    QRegExp freqListRegEx("^(\\d+)\\s([0-9]*\\.[0-9]+|[0-9]+)\\s(.+)");
    QTextStream stream(&file);
    QString line;
    do {
        line = stream.readLine();
        freqListRegEx.indexIn(line);
        QStringList list = freqListRegEx.capturedTexts();
        //qDebug() << "line <" << line << ">  list:" << list;

        if (list.size() != 4) {
            qDebug() << "error, found " << list.size();
            continue;
        }
        if (list[1].isEmpty()) continue;

        //qDebug() << " word: " << list[3] << " rank: " << list[1];
        rankDict[list[3]] = list[1].toUInt();

    } while (!line.isNull());

    return true;
}


static bool parseCedictFile(const QHash<QString, int>& rankDict, const QString& path)
{

    QFile file(path);

    if (!file.open (QIODevice::ReadOnly))
    {
        qWarning() << "Failed to open:" << file.fileName();
        return false;
    }

    uint rankedWords = 0;
    uint unrankedWords = 0;

    QRegExp cedictRegEx("^([^\\s]+)\\s+([^\\s]+)\\s+\\[(.+)\\]\\s+\\/(.+)\\/\\s*$");
    QTextStream stream(&file);
    QString line;
    do {
        line = stream.readLine();
        cedictRegEx.indexIn(line);
        QStringList list = cedictRegEx.capturedTexts();
        //qDebug() << "line <" << line << ">  list:" << list;
        if (list.size() != 5) {
            qDebug() << "error, found " << list.size();
            continue;
        }

        if (list[1].isEmpty()) continue;
/*
        qDebug() << "traditional: " << list[1];
        qDebug() << "simplified: " << list[2];
        qDebug() << "pinyin: " << list[3];
        qDebug() << "english: " << list[4];
*/

        QString numberedPinyin = list[3];

        qDebug() << "\nnumbered pinyin: " << numberedPinyin;

        //QString toneNums = extractToneNumbers(numberedPinyin);

        QString displayPinyin; //e.g. "Ài ěr lán", "shí tou, jiǎn zi bù"
        QString tonemarkedSearchPinyin; //used for search. e.g. "àiěrlán", "shítoujiǎnzibù"
        QString tonelessSearchPinyin; //used for search.  e.g. "aierlan", "shitoujianzibu"
        QString toneNums; //comma separated tone numbers (1-5)
        QString componentPinyin; //comma separated tonemarked pinyin components e.g. shí,tou,jiǎn,zi,bù
        parseCedictEntry(numberedPinyin,
                         displayPinyin,
                         tonemarkedSearchPinyin,
                         tonelessSearchPinyin,
                         toneNums,
                         componentPinyin);
/*
        qDebug() << "displayPinyin: " << displayPinyin;
        qDebug() << "tonemarkedSearchPinyin: " << tonemarkedSearchPinyin;
        qDebug() << "tonelessSearchPinyin: " << tonelessSearchPinyin;
*/
        //need to convert inline pinyin in english defs
        //wrapped in [ ], may be more than one instance


        //e.g.
        //"(traditional) Chinese medicine/CL:服[fu4],種|种[zhong3]"
        //"doctor/CL:個|个[ge4],位[wei4],名[ming2]"  -> "doctor/CL:個|个 gè,位 wèi,名 míng"

        QRegExp inlinePinyinMatch("\\[((?:[\\w+\\:] *)+)\\]");
        int pos = 0;
        int endOfLastMatch = 0;
        QString rawEnglish = list[4];

        //qDebug() << "rawEnglish: " << rawEnglish;
        QString pinyinisedEnglish;
        while ((pos = inlinePinyinMatch.indexIn(rawEnglish, pos)) != -1) {
            //copy english up until the point of the match
            pinyinisedEnglish += rawEnglish.mid(endOfLastMatch, pos-endOfLastMatch);
            QString converted = makeDisplayPinyin(inlinePinyinMatch.cap(1));
            qDebug() << " converted " << inlinePinyinMatch.cap(1) << " to " << converted;
            pinyinisedEnglish += " " + converted;
            pos += inlinePinyinMatch.matchedLength();
            endOfLastMatch = pos;
        }
        pinyinisedEnglish += rawEnglish.mid(endOfLastMatch, rawEnglish.length()-endOfLastMatch);

        //now split up the definitions and extract/remove the "special"
        //fields from them - "CL", "also written "
        QStringList englishDefEntries = pinyinisedEnglish.split("/");

        //we put the non-special definitions back into this string
        QString english;

        QString classifiers;
        QString alsoWritten;
        QString alsoPronounced;

        int i;
        for (i=0; i < englishDefEntries.count(); i++) {
            QString def = englishDefEntries[i];

            QRegExp classifierMatch("CL:");
            QRegExp alsoWrittenMatch("^also written ");
            QRegExp hanziPairMatch("(([\\x4e00-\\x9fa5]+)|([\\x4e00-\\x9fa5]+)\\|([\\x4e00-\\x9fa5]+))");

            QRegExp alsoPronouncedMatch("(also pronounced )|(also pron. )|(also pr. )");
            //check for and extract any classifier definitions
            //"a well/CL:口[kou3]/neat/orderly
            //"orange juice/CL:瓶[ping2],杯[bei1],罐[guan4],盒[he2]/see also 橙汁[cheng2 zhi1]"
            if ((pos = classifierMatch.indexIn(def)) != -1) {
                //qDebug() << "CL match";
                int count = 0;
                while ((pos = hanziPairMatch.indexIn(def, pos)) != -1) {
                    QString pair = hanziPairMatch.cap(1);
                    //we only need one, so if it is a trad|simp pair we just use the traditional
                    QString tradClassifier = pair.split("|")[0];
                    //qDebug() << " " << tradClassifier;
                    if (count++ > 0) classifiers += ",";
                    classifiers += tradClassifier;
                    pos += hanziPairMatch.matchedLength();
                }

            } else if ((pos = alsoWrittenMatch.indexIn(def)) != -1)  {
                //check for and extract any "also written" definitions

                //also written 機槍|机枪/machine gun
                //"moisturizer (cometics) also written 保溼[bao3 shi1]"
                //to concede/to admit defeat/also written 服輸|服输
                //"to attend to/to care for (patients etc)/to look after/to wait upon/to serve/also written 服侍, see also 服事[fu2 shi4]"

                int count = 0;
                while ((pos = hanziPairMatch.indexIn(def, pos)) != -1) {
                    QString pair = hanziPairMatch.cap(1);
                    if (count++ > 0) alsoWritten += ",";
                    alsoWritten += pair;
                    pos += hanziPairMatch.matchedLength();
                }

            } else if ((pos = alsoPronouncedMatch.indexIn(def)) != -1)  {
                pos = alsoPronouncedMatch.indexIn(def, pos) + alsoPronouncedMatch.matchedLength();
                alsoPronounced = def.mid(pos, def.length()-pos);
                //qDebug() << "!!!also pronounced " << alsoPronounced;
            } else {
                if (i > 0) english += "/";
                english += def;
            }


        }

        //qDebug() << "classifiers " << classifiers;

        //qDebug() << "alsoWritten: " << alsoWritten;
        //qDebug() << "english: " << english;


        uint rank = 999999;
        if (rankDict.contains(list[2])) {
            rank = rankDict.value(list[2]);
            //qDebug() << " ranked as " << rank;
            rankedWords++;
        } else {
            //qDebug() << " no rank entry for " << list[2];
            unrankedWords++;
        }
        bool ok = addWord(list[1], list[2],
                displayPinyin, tonemarkedSearchPinyin, tonelessSearchPinyin,
                english,
                alsoWritten,
                alsoPronounced,
                classifiers,
                toneNums,
                componentPinyin,
                rank);
        assert(ok);
        //qDebug();
    } while (!line.isNull());

    qDebug() << "rankedWords: " << rankedWords << " unrankedWords: " << unrankedWords;
    return true;
}


bool createDb(const char* dbPath, const char* cedictPath, const char* rankFilePath)
{
    int ret = sqlite3_open_v2(dbPath, &s_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
    if ((ret != SQLITE_OK) || (s_db == NULL)) {
        qDebug() << "Error opening database";
        return false;
     }


    char *errmsg;
    ret = sqlite3_exec(s_db,
        "CREATE VIRTUAL TABLE words using FTS3 ( "
               "traditional text,"
               "simplified text,"
               "pinyin text,"
               "pinyin_spaceless text,"
               "pinyin_toneless text,"
               "english text,"
               "also_written_as text,"
               "also_pronounced_as text,"
               "classifiers text,"
               "tone_nums text,"
               "component_pinyin text,"
               "word_rank integer);",
         NULL, 0, &errmsg);

    if(ret != SQLITE_OK) {
        qDebug() << "Error creating words table: " << errmsg;
      return false;
    }

    qDebug() << "created ok";


    QString query = QString("INSERT INTO words VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    QByteArray queryUtf8 = query.toUtf8();

    ret = sqlite3_prepare_v2(s_db,
        queryUtf8.constData(),
        queryUtf8.size(),
        &s_addWordStmt,
        NULL);

    QHash<QString, int> rankDict;
    parseFreqListFile(rankDict, rankFilePath);

    parseCedictFile(rankDict, cedictPath);

    sqlite3_finalize(s_addWordStmt);
    sqlite3_finalize(s_addClassifierUseStmt);
    sqlite3_close(s_db);
    s_db = NULL;
    return true;
}

