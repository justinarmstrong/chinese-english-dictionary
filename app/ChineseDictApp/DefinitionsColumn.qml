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

import QtQuick 2.9
import QtQuick.Controls 2.2

import "UIConstants.js" as UI
import "utils.js" as UTILS

Column {
    id: main
    width: parent.width

    property variant definitions
/*
    Item {
        width: parent.width
        height: 4
    }
 */
    Repeater {
        id: repeater
        model: definitions

        Item  {
            width: main.width
            height: Math.max(englishField.height, hanziColumn.height)+6

            Row {
                height: parent.height
                id: row1
                spacing: 4
                width: main.width
                Column {
                    width: 60
                    id: hanziColumn
                    spacing: 0
                    ChineseDef {
                        width: main.width
                        id: hanziField
                        clickWidth: row1.width
                        clickHeight: row1.height
                        toneNums: modelData.toneNums
                        text: settings.useTraditional? modelData.traditional : modelData.simplified
                        textHeight:UI.FONT_LARGE
                    }

                    Label {
                        width: main.width
                        id: pinyinField
                        font.pixelSize: UI.FONT_SMALL
                        text: UTILS.toneColorOnPinyin(modelData.toneNums, modelData.pinyin)
                        color: UI.DEFAULT_TEXT_COLOR
                        textFormat: settings.toneColorsEnabled ? Text.RichText : Text.PlainText
                        elide: Text.ElideRight
                    }
                }
                Label {
                    textFormat: Text.PlainText
                    id: englishField
                    text: modelData.english
                    color: UI.DEFAULT_TEXT_COLOR
                    font.pixelSize: UI.FONT_SMALL
                    width: row1.width - x
                    wrapMode: Text.Wrap
                }

            }
            x:-1000
            Component.onCompleted: {
                x = 0
            }/*
            onVisibleChanged: {
                if (!visible) x = -1000;
                else x = 0;
            }*/
            Behavior on x {
                NumberAnimation {
                    easing.type: Easing.InOutElastic
                    duration: 700 + (index * 80)
                    easing.amplitude: 1.0; easing.period: 1.5
                    easing.overshoot: 10
                }
            }
        }
    } // repeater

}


