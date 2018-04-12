/*
 * Copyright Justin Armstrong 2012, 2018. All rights reserved.
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
 */

function splitTradSimp(chinese, wantTraditional) {
    var variants = chinese.split("|");
    if (variants.length < 2) return chinese;
    if (wantTraditional) return variants[0];
    return variants[1];
}

function colorForToneNumber(toneNum) {

    switch (toneNum) {
    case 1: return settings.tone1Color;
    case 2: return settings.tone2Color;
    case 3: return settings.tone3Color;
    case 4: return settings.tone4Color;
    }
    return settings.tone5Color;

}

function toneColorOnPinyin(toneNums, pinyin)
{
    if (!settings.toneColorsEnabled) return pinyin;

    var colored = ""
    //console.log("pinyin before: " + pinyin);
    var toneNumList = toneNums.split(",");
    //console.log("toneNums: " + toneNums);
    var wordList = pinyin.split(" ")
    var wordIndex;
    var toneIndex = 0;
    for (wordIndex = 0; wordIndex < wordList.length; wordIndex++){
        if (!wordList[wordIndex].match(/^\w$|、|・|·|,|，/)) {
            colored += "<font color='" +
                    colorForToneNumber(parseInt(toneNumList[toneIndex])) +
                    "'>" + wordList[wordIndex] + "</font>"
            colored += " "
            //console.log("colorising " + wordList[wordIndex])
            toneIndex++;
        } else {
            //console.log("skipping " + wordList[wordIndex])
            colored += wordList[wordIndex] + " ";
        }
    }

    //console.log("pinyin  " + pinyin + " <-- " + toneNums);
    //console.log("pinyin now: " + colored);
    return colored;
}


function TextColoriser(toneNumString) {
    //console.log("TextColoriser toneNums: " + toneNumString)
    this.toneNumList = toneNumString.split(",")
    this.toneNumIndex = 0

    this.coloriseText = function(word) {
        if (word.match(/^\w$|、|・|·|,|，/)) {
            //console.log("skipping " + word)
            return word //don't colour
        }

        if (this.toneNumIndex >= this.toneNumList.length) {
            //console.log("error, <" + word + "> toneNumIndex " + this.toneNumIndex + " >= " + this.toneNumList.length)
            return word
        }

        //console.log(this.toneNumIndex)
        return "<font color='" +
                colorForToneNumber(parseInt(this.toneNumList[this.toneNumIndex++])) +
                "'>" + word + "</font>"
    }

}

function toneColorOnCharacters(toneNums, characters)
{
    if (!settings.toneColorsEnabled) return characters;
    if (toneNums.length === 0) return characters;
    //console.log("characters before: " + characters + " len:" + characters.length + " toneNums:" + toneNums)
    var coloriser = new TextColoriser(toneNums)
    var colored = ""
    //need to cope with multibyte characters, so string.split() doesn't work!
    var charIndex;
    var toneIndex = 0;
    for (var byteIndex = 0; byteIndex < characters.length; byteIndex++){
        var charCode = characters.charCodeAt(byteIndex);
        if (charCode >= 0xD800 && charCode <= 0xDBFF) {
            charCode = characters.charCodeAt(byteIndex + 1);
            if (charCode >= 0xDC00 && charCode <= 0xDFFF) {
                colored += coloriser.coloriseText(characters.slice(byteIndex, byteIndex + 2))
                ++byteIndex
                continue
            }
            colored += coloriser.coloriseText(characters.charAt(byteIndex))
        }
        colored += coloriser.coloriseText(characters.charAt(byteIndex))
    }
    return colored;

}
