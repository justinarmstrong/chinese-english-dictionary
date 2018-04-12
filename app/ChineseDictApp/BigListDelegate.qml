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

SwipeDelegate {
    id: itemDelegate
    anchors.left: parent.left
    anchors.right: parent.right

    property string pinyin
    property string hanzi
    property bool beingRemoved

    function assignChinese() {
        pinyin = UTILS.toneColorOnPinyin(object.toneNums, object.pinyin)
        hanzi =  settings.useTraditional? object.traditional : object.simplified
        hanzi = UTILS.toneColorOnCharacters(object.toneNums,
                                             settings.useTraditional? object.traditional : object.simplified)

    }

    Component.onCompleted: assignChinese()

    Connections {
        target: settings
        onUseTraditionalChanged: assignChinese()
        onToneColorsEnabledChanged: assignChinese()
        onTone1ColorChanged: assignChinese()
        onTone2ColorChanged: assignChinese()
        onTone3ColorChanged: assignChinese()
        onTone4ColorChanged: assignChinese()
        onTone5ColorChanged: assignChinese()

    }

    contentItem: Column {
        id: col
        anchors.verticalCenter: parent.verticalCenter
        spacing: 2
        Label {
            id: hanziField
            text: hanzi
            font.pixelSize: UI.FONT_LARGE
            elide: Text.ElideRight
            width: parent.width - anchors.rightMargin
            lineHeightMode: Text.FixedHeight
            lineHeight: font.pixelSize  //* 1.25
            color: UI.DEFAULT_TEXT_COLOR
        }
        Label {
            font.pixelSize: UI.FONT_XSMALL
            lineHeightMode: Text.FixedHeight
            lineHeight: font.pixelSize  //* 1.25
            id: pinyinField
            text: pinyin
            color: UI.LIST_SUBTITLE_COLOR //ignored if tone colouring turned on
            elide: Text.ElideRight
            width: parent.width - anchors.rightMargin
        }
        Label {
            font.pixelSize: UI.FONT_XSMALL
            lineHeightMode: Text.FixedHeight
            lineHeight: font.pixelSize //* 1.25
            font.weight: Font.Normal
            id: englishField
            text: object.english
            color: UI.DEFAULT_TEXT_COLOR
            elide: Text.ElideRight
            width: parent.width - anchors.rightMargin
        }
    }

    //text:hanzi + object.english

    swipe.right: Rectangle {
        width: parent.width
        height: parent.height

        clip: true
        color: SwipeDelegate.pressed ? "#555" : "#666"

        Label {
            text: qsTr("Removed - click to undo")
            color: "white"

            //padding: 20
            anchors.fill: parent
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter

            opacity: itemDelegate.swipe.complete ? 1 : 0
            Behavior on opacity { NumberAnimation { duration: 200 } }
        }

        SwipeDelegate.onClicked: itemDelegate.swipe.close()
        SwipeDelegate.onPressedChanged: undoTimer.stop()
    }

    Timer {
        id: undoTimer
        interval: 800
        onTriggered: {
            settings.removeFavourite(object.wordsKey, index)
        }
    }

    swipe.onCompleted: undoTimer.start()

    onClicked: {
        requestDetailsAsync(object.wordsKey)
        appWindow.pageStack.push(detailsPage,
                                 {
                                     wordsKey: object.wordsKey,
                                     simplified: object.simplified,
                                     traditional: object.traditional,
                                     pinyin: object.pinyin,
                                     toneNums: object.toneNums,
                                     rawEnglish: object.english,
                                     listRowIndex: index
                                 })
    }

}
