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

Row {
    property string title
    property string text
    property color titleColor: UI.COLOR_SECONDARY_FOREGROUND
    property int fontHeight: UI.FONT_SMALL
    property alias isHanzi : cd.isHanzi
    property alias toneNums: cd.toneNums

    Label {
        id: label
        textFormat: Text.PlainText
        text: title
        font.pixelSize: fontHeight
        font.weight: Font.Normal
        color: titleColor
        wrapMode: Text.Wrap
    }

    ChineseDef {
        id: cd
        width: parent.width - label.width
        text: parent.text
        textHeight: fontHeight
    }
}


