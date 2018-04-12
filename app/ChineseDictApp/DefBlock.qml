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

import QtQuick 2.6
import QtQuick.Controls 2.1

import "UIConstants.js" as UI

Column {
    id: defBlock
    property string title
    property color titleColor: UI.COLOR_SECONDARY_FOREGROUND
    property int animationDuration: 600
    Label {
        width: parent.width
        font.weight: Font.Bold
        font.pixelSize: UI.FONT_LSMALL
        color: titleColor
        text: title
    }
    spacing: 0

/*
    x:-1000
    state:"OFFSCREEN"
    states: [
        State {
            name: "ONSCREEN"
            PropertyChanges { target: defBlock; x:0 }
        },
        State {
            name: "OFFSCREEN"
            PropertyChanges { target: defBlock; x:-1000 }
        }

    ]
*/
    property int pageStatus

    onPageStatusChanged: {
        if (pageStatus === PageStatus.Inactive) {
            state = "OFFSCREEN"
        } else {
            state = "ONSCREEN"
        }
    }

    transitions: [
        Transition {
            from: "OFFSCREEN"
            to: "ONSCREEN"
            NumberAnimation {
                properties: "x"
                easing.type: Easing.InOutElastic
                duration: animationDuration
                easing.amplitude: 1; easing.period: 1.5
                easing.overshoot: 1
            }
        }
    ]

}

