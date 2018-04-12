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

Item {
    id: def
    property string text
    property int textHeight
    property bool isHanzi: true
    property string toneNums

    height: edit.height
    width: edit.paintedWidth

    Rectangle {
        radius: 4
        color: "#cecbce"
        smooth: true
        //border.color: "#22000000"
        //border.width: 3

        x: edit.x - 4
        width: edit.paintedWidth + 8
        height: edit.height
        opacity: clickArea.pressed ? 1 : 0

        Behavior on opacity {
            NumberAnimation {
                duration: 300
            }
        }
    }

    property int clickWidth: edit.width
    property int clickHeight: edit.height

    TextEdit {
        id: edit
        clip: false
        focus: true
        text: isHanzi ? UTILS.toneColorOnCharacters(toneNums, parent.text) :
                        UTILS.toneColorOnPinyin(toneNums, parent.text)

        textFormat: settings.toneColorsEnabled ? Text.RichText : Text.PlainText
        color: UI.DEFAULT_TEXT_COLOR
        font.weight: Font.Normal
        font.pixelSize: textHeight
        wrapMode: Text.Wrap

        width: parent.width
        readOnly: true

        MouseArea {
            id: clickArea
            width: clickWidth
            height: clickHeight
            onPressAndHold: {
                contextMenu.open()
            }
        }
    } //edit

    TextEdit {
        id: invisibleEdit
        visible: false
        textFormat: Text.PlainText
        text: parent.text
        readOnly: true
    } //edit

    Menu {
        id: contextMenu
        //visualParent: pageStack
        MenuItem {
            text: "Copy '" + invisibleEdit.text + "'"
            onClicked: {
                invisibleEdit.selectAll()
                invisibleEdit.copy()
                showMessage("Copied '" + invisibleEdit.selectedText + "' to clipboard");
                invisibleEdit.deselect()
            }
        }

        MenuItem {
            text: "Search for '" + invisibleEdit.text + "'"
            onClicked: {
                pageStack.pop()
                swipeView.currentIndex = 0 //yuk
                searchPage.switchToChineseAndSearch(invisibleEdit.text)
            }
        }

    } // contextMenu


} //Item
