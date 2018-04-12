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
#include <QRegularExpression>
#include <QStringList>

#include <assert.h>
#include "textutils.h"

#define ARRAY_SIZE(arr) ((sizeof(arr))/(sizeof(arr[0])))

//matches pinyin syllable + optional tone num (e.g. wei2)
//static QRegularExpression s_pinyinSyllableRegExp(u8"([0-9]|[A-zÜü:]+)([1-5])?");
static QRegularExpression s_pinyinSyllableRegExp(u8"([0-9]|[a-zA-Z\u00C0-\u024F:]+)([1-5])?");

//matches pinyin syllable + required tone num
static QRegularExpression s_pinyinSyllableNumRegExp(u8"([0-9]|[A-zÜü:]+)([1-5])+");

//matches tonemarked pinyin
static QRegularExpression s_toneMatchRegExp(u8"[āōēīūǖáóéíúǘǎǒěǐǔǚàòèìùǜĀŌĒĪŪǕÁÓÉÍÚǗǍǑĚǏǓǙÀÒÈÌÙǛ]+");
//matches an actual hanzi character
static QRegularExpression s_hanziMatchRegExp("\\p{Han}");
static QRegularExpression s_toneNumMatchRegExp("[1-5]");
static QRegularExpression s_punctuationRegExp("[,·、]");

struct ToneMarkLocationMap_t {
    QString str;
    uint index;

};

static ToneMarkLocationMap_t s_toneMarkLocationMaps[] =
{

    { QString::fromWCharArray(L"ai"), 0 }, //"a*i"
    { QString::fromWCharArray(L"ao"), 0 }, //"a*o"
    { QString::fromWCharArray(L"ei"), 0 }, //"e*i"
    { QString::fromWCharArray(L"ou"), 0 }, //"o*u"
    { QString::fromWCharArray(L"ia"), 1 }, //"ia*"
    { QString::fromWCharArray(L"ie"), 1 }, //"ie*"
    { QString::fromWCharArray(L"iu"), 1 }, //"iu*"
    { QString::fromWCharArray(L"io"), 1 }, //"io*"
    { QString::fromWCharArray(L"ua"), 1 }, //"ua*"
    { QString::fromWCharArray(L"ue"), 1 }, //"ue*"
    { QString::fromWCharArray(L"ui"), 1 }, //"ui*"
    { QString::fromWCharArray(L"uo"), 1 }, //"uo*"
    { QString::fromWCharArray(L"üe"), 1 }, //"üe*"
    { QString::fromWCharArray(L"a"),  0 },
    { QString::fromWCharArray(L"e"),  0 },
    { QString::fromWCharArray(L"i"),  0 },
    { QString::fromWCharArray(L"o"),  0 },
    { QString::fromWCharArray(L"u"),  0 },
    { QString::fromWCharArray(L"ü"),  0 }

};



static wchar_t s_toneMarks[5][13] =
{
    L"aoeiuü" L"AOEIUÜ",  //src
    L"āōēīūǖ" L"ĀŌĒĪŪǕ", //1st tone
    L"áóéíúǘ" L"ÁÓÉÍÚǗ", //2nd tone
    L"ǎǒěǐǔǚ" L"ǍǑĚǏǓǙ", //3rd tone
    L"àòèìùǜ" L"ÀÒÈÌÙǛ"  //4th tone
};

bool isHanzi(const QString& text)
{
    return s_hanziMatchRegExp.match(text).hasMatch();
}

bool isPunctuation(const QString& text)
{
   return s_punctuationRegExp.match(text).hasMatch();
}

textFormat_t determineTextFormat(const QString& text)
{
    //qDebug() << text;
    if (isHanzi(text)) {
        //qDebug() << "is apparently hanzi";
        return tfHanzi;
    } else if (s_toneMatchRegExp.match(text, QRegularExpression::CaseInsensitiveOption).hasMatch()) {
        //qDebug() << "is apparently tonemarked";
        return tfPinyinTonemarks;
    } else if (s_pinyinSyllableNumRegExp.match(text).hasMatch()) {
        //qDebug() << "is apparently numbered";
        return tfPinyinNumbers;
    } else {
        //qDebug() << "is apparently unnumbered pinyin";
        return tfPinyinNoTones;
    }
}



/*
QList<int> getToneNumbersFromMarkedString(const QString& toneMarked)
{
    QList<int> toneNums;
    int i;

    int length = toneMarked.length();

    for (i=0; i < length; i++) {
        int tone;
        for (tone = 0; tone < 4; tone++) {
            if (toneMatches[tone].contains(toneMarked.at(i))) {
                toneNums.append(tone+1); //numbered from 1
            }
        }
    }
    qDebug() << "found the tones " << toneNums << " from " << toneMarked;
    return toneNums;
}
*/


static void _insertToneMark(QString& syllable, int toneNum)
{
    if ((toneNum > 0) && (toneNum < 5)) { //got a useful tone number
        //now find where we should put the tone mark
        int markPos = -1;
        uint i;
        ToneMarkLocationMap_t* locMap;
        QString lower = syllable.toLower();
        for (i=0; (i < ARRAY_SIZE(s_toneMarkLocationMaps)) && (markPos < 0); i++) {
            locMap = &s_toneMarkLocationMaps[i];
            markPos = lower.indexOf(locMap->str, Qt::CaseInsensitive);
            //qDebug() << "looking for " << locMap->str << " in " << syllable << ", found? " << markPos;
        }

        //replace the char at markPos with a marked one
        if (markPos > -1) {
            //find the replacement char
            wchar_t srcChar = syllable.at(markPos + locMap->index).unicode();
            int toneMarkIndex = -1;
            for (i=0; i < ARRAY_SIZE(s_toneMarks[0]); i++) {
                if (s_toneMarks[0][i] == srcChar) {
                    toneMarkIndex = i;
                    break;
                }
            }
            Q_ASSERT(toneMarkIndex != -1);
            wchar_t toneMarkChar = s_toneMarks[toneNum][toneMarkIndex];

            //qDebug() << "replacing " << QChar(srcChar) << " with " << QChar(toneMarkChar) << " at " << toneMarkIndex;
            syllable.replace(markPos + locMap->index, 1, QChar(toneMarkChar));
        }
    } //toneNum between 1 and 5
}

static QString _makeSearchPinyin(const QString& words, bool withToneMarks)
{

    QString cleaned = words;
    //qDebug() << "_makeSearchPinyin: " << cleaned << (withToneMarks ? "(with tonemarks)" : "(no tonemarks)");
    QString result;

    QRegularExpressionMatchIterator matches = s_pinyinSyllableRegExp.globalMatch(cleaned);
    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        QString syllable;
        int toneNum = 5;
        //qDebug() << "match.lastCapturedIndex() " << match.lastCapturedIndex();
        if (match.lastCapturedIndex() > 1) {
            QString toneNumStr = match.captured(2);
            toneNum = toneNumStr.at(0).digitValue();
        }
        syllable = match.captured(1).toLower();
        syllable.replace(QString("u:"), QChar(L'ü'));
        if (syllable.length() > 1) {
            //if single character, might actually be matching 'V' e.g. VCR
            syllable.replace(QString("v"), QChar(L'ü'));
        }
        if (!withToneMarks){
            //the non-tonemarked search treats ü and u identically,
            //so we need to replace all ü with u!
            syllable.replace(QString::fromWCharArray(L"ü"), QChar('u'));
        }
        if (withToneMarks) _insertToneMark(syllable, toneNum);
        //qDebug() << syllable << ":" << toneNum;

        result += syllable;
    }

    //qDebug() << "made: " << result;
    return result;
}


QString makeTonelessSearchPinyin(const QString& words)
{
    return _makeSearchPinyin(words, false);
}

QString makeToneMarkedSearchPinyin(const QString& words)
{
     return _makeSearchPinyin(words, true);
}


void parseCedictEntry(const QString& source, //input
                      QString& displayPinyin,           //out
                      QString& tonemarkedSearchPinyin,  //out
                      QString& tonelessSearchPinyin,    //out
                      QString& toneNums,                //out
                      QString& componentPinyin)         //out
{
    qDebug() << "parseCedictEntry: " << source;

    QString cleaned = source;
    displayPinyin.clear();
    tonemarkedSearchPinyin.clear();
    tonelessSearchPinyin.clear();
    toneNums.clear();
    componentPinyin.clear();
    QStringListIterator iterator = QStringListIterator(cleaned.split(" "));
    while (iterator.hasNext()) {
        QString syllable = iterator.next();
        //qDebug() << "syllable: " << syllable;

        int toneNum = 5;
        if (syllable.length() > 1) {
            int digit = syllable.at(syllable.length()-1).digitValue();
            if ((digit >= 1) && (digit <= 5)) {
                toneNum = digit;
                syllable.chop(1); //remove number
            }
        }

        syllable.replace(QString("u:"), QChar(L'ü')).replace(QString("U:"), QChar(L'Ü'));
        if (syllable.length() > 1) {
            //if single character, might actually be matching 'V' e.g. VCR
            syllable.replace(QString("v"), QChar(L'ü')).replace(QString("V"), QChar(L'Ü'));
        }
        QString tonemarkedSyllable = syllable;
        _insertToneMark(tonemarkedSyllable, toneNum);

        QString tonelessSyllable = syllable.toLower();
        //the non-tonemarked search treats ü and u identically,
        //so we need to replace all ü with u!
        tonelessSyllable.replace(QString::fromWCharArray(L"ü"), QChar('u'));

        if (syllable == "·") {
            //qDebug() << "appending dot";
            displayPinyin += " ·";
        } else if (syllable == ",") {
            displayPinyin += ",";
        } else {
            //single character items are numbers or letter of acronyms, so don't put spaces between them
            if ((syllable.length() > 1) && (!displayPinyin.isEmpty())) {
                displayPinyin += " ";
            }
            displayPinyin += tonemarkedSyllable;
            tonemarkedSearchPinyin += tonemarkedSyllable.toLower();
            tonelessSearchPinyin += tonelessSyllable;

            toneNums += QString::number(toneNum);
            componentPinyin += tonemarkedSyllable.toLower();
            if (iterator.hasNext()) {
                toneNums += ",";
                componentPinyin += ",";
            }
        }

    } // iterator
    qDebug() << "displayPinyin:" << displayPinyin;
    qDebug() << "tonemarkedSearchPinyin:" << tonemarkedSearchPinyin;
    qDebug() << "tonelessSearchPinyin:" << tonelessSearchPinyin;
    qDebug() << "toneNums:" << toneNums;
    qDebug() << "componentPinyin:" << componentPinyin;
    assert(toneNums.split(',').length() == componentPinyin.split(',').length());
}


//wrapper function, mainly for testing convenience
QString makeDisplayPinyin(const QString source) {
    QString displayPinyin, tonemarkedSearchPinyin, tonelessSearchPinyin, toneNums, componentPinyin;
    parseCedictEntry(source, displayPinyin, tonemarkedSearchPinyin, tonelessSearchPinyin, toneNums, componentPinyin);
    assert(makeToneMarkedSearchPinyin(source) == tonemarkedSearchPinyin);
    assert(makeTonelessSearchPinyin(source) == tonelessSearchPinyin);
    return displayPinyin;
}
