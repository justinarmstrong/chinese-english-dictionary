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

#ifndef CONVERSION_UTILS_H
#define CONVERSION_UTILS_H

#include <QObject>
#include <QList>

bool isHanzi(const QString& text);
bool isPunctuation(const QString& text);
QString extractToneNumbers(const QString& words);

void parseCedictEntry(const QString& source, //input
                      QString& displayPinyin,           //out
                      QString& tonemarkedSearchPinyin,  //out
                      QString& tonelessSearchPinyin,    //out
                      QString& toneNums,                //out
                      QString& componentPinyin);        //out
QString makeDisplayPinyin(const QString source) ;
QString makeTonelessSearchPinyin(const QString& words);
QString makeToneMarkedSearchPinyin(const QString& words);
//QList<int> getToneNumbersFromMarkedString(const QString& toneMarked);


typedef enum {
    tfHanzi,
    tfPinyinNoTones,
    tfPinyinNumbers,
    tfPinyinTonemarks
} textFormat_t;

textFormat_t determineTextFormat(const QString& text);

#endif // PINYINUTILS_H
