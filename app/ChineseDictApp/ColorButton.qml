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
import QtQuick.Dialogs 1.1

import "UIConstants.js" as UI

Column {
    id: button
    property string text
    property color color
    property color selectedColor

    Component.onCompleted: selectedColor = color

    height: rect.height + rect.border.width + (spacing*2) + label.height
    spacing: 2

    Label {
        id: label
        anchors.left: parent.left
        anchors.right: parent.right
        horizontalAlignment: Text.AlignHCenter
        text: button.text
        font.weight: Font.Bold
        font.pixelSize: UI.FONT_XXSMALL
    }

    Rectangle {
        id: rect
        radius: 4
        color: mouseArea.pressed ? Qt.darker(button.color, 3) : button.color
        smooth: true
        clip: true

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: {
                colorDialog.open()
            }
        }

        CustomColorDialog {
            id: colorDialog
            text: button.text + " tone colour"
            selectedColor: button.color

            onAccepted: {
                button.selectedColor = colorDialog.selectedColor
                button.color = colorDialog.selectedColor
            }
        }


        border.color: "#22000000"
        border.width: 6

        height: 40
        width: 40
    }

}
