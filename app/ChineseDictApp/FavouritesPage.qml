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

Pane {

    //orientationLock: PageOrientation.LockPortrait
    anchors.margins: 0
    padding: 0
    clip: true
    Item {
        id: mainCol
        width: parent.width
        height: parent.height
        //spacing: 0

        Label {
            textFormat: Text.PlainText
            visible: settings.favouritesList.count == 0
            text: "No saved definitions"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            //height: parent.height - (settingsHeader.y + settingsHeader.height)
            //font.family: UI.FONT_FAMILY_BOLD
            font.weight: Font.Normal
            font.pixelSize: UI.FONT_DETAILS
            fontSizeMode: Text.Fit
            color: UI.COLOR_SECONDARY_FOREGROUND
        }


        ListView {
            visible: settings.favouritesList.count > 0
            id: listView
            clip: true
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            //height: parent.height - (settingsHeader.y + settingsHeader.height)
            spacing: 0

            model: settings.favouritesList
            delegate:  BigListDelegate {}

            ScrollIndicator.vertical: ScrollIndicator { }


            remove: Transition {
                SequentialAnimation {
                    PauseAnimation { duration: 125 }
                    NumberAnimation { property: "height"; to: 0; easing.type: Easing.InOutQuad }
                }
            }

            displaced: Transition {
                SequentialAnimation {
                    PauseAnimation { duration: 125 }
                    NumberAnimation { property: "y"; easing.type: Easing.InOutQuad }
                }
            }

        }

    }
}
