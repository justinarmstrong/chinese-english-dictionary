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

    //anchors.fill: parent
    anchors.margins: 0
    padding: 0

    Column {
        id: mainCol
        width: parent.width
        height: parent.height
        Flickable {
            anchors.left: parent.left
            anchors.right: parent.right
            clip: true
            id: flickableArea
            height: parent.height
            contentHeight: contentCol.y + contentCol.height
            flickableDirection: Flickable.VerticalFlick
            boundsBehavior: Flickable.StopAtBounds
            Column {
                id: contentCol
                anchors.left: parent.left
                anchors.right: parent.right
                Image {
                    width: parent.width
                    source:"background.png"
                    fillMode: Image.Stretch
                    Label {
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: UI.MARGIN_XLARGE
                        anchors.rightMargin: UI.MARGIN_XLARGE
                        anchors.topMargin: UI.MARGIN_XLARGE
                        horizontalAlignment: Text.AlignLeft
                        text: "Chinese-English Dictionary"
                        font.pixelSize: UI.FONT_DETAILS
                        fontSizeMode: Text.Fit
                        font.weight: Font.Bold
                        color: "white"
                    }
                    Column {
                        anchors.bottom: parent.bottom
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.leftMargin: UI.MARGIN_XLARGE
                        anchors.rightMargin: UI.MARGIN_XLARGE
                        anchors.bottomMargin: UI.MARGIN_XLARGE

                        spacing: 4
                        Label {
                            anchors.right: parent.right
                            horizontalAlignment: Text.AlignLeft
                            text: "Version 1.1"
                            font.pixelSize: 18
                            fontSizeMode: Text.Fit
                            color: "white"
                        }
                        Label {
                            width: parent.width
                            horizontalAlignment: Text.AlignLeft
                            color: "white"
                            font.pixelSize: 16
                            fontSizeMode: Text.Fit
                            text: "© 2012, 2018 Justin Armstrong<br>" +
                                  "<a href='https://github.com/justinarmstrong/chinese-english-dictionary'>Application website</a><br>" +
                                  "Uses dictionary information from the " +
                                  "CC-CEDICT project"
                            onLinkActivated: {
                                Qt.openUrlExternally(link);
                            }
                        }
                    }
                }

                Column {
                    id: settingsCol
                    anchors.left: parent.left
                    anchors.leftMargin: UI.MARGIN_XLARGE
                    anchors.right: parent.right
                    anchors.rightMargin: UI.MARGIN_XLARGE
                    spacing: UI.PADDING_XLARGE

                    Item {
                        width: parent.width
                        height: UI.PADDING_XLARGE
                    }
                    Column {
                        id: optionsCol  //FIXME needed?
                        width: parent.width
                        //spacing: UI.PADDING_SMALL
                        Label {
                            text: "Characters in search results"
                            //width: parent.width
                            font.weight: Font.Bold
                            //lineHeightMode: Text.FixedHeight
                            //lineHeight: font.pixelSize * 1.3
                        }
                        ButtonGroup {
                            buttons: characterChoice.children
                            onClicked: {
                                settings.useTraditional = buttonTraditional.checked
                            }
                        }

                        Row {
                            id: characterChoice
                            padding: 0
                            RadioButton {
                                id : buttonSimplified
                                checked: !settings.useTraditional
                                text: qsTr("Simplified")
                            }
                            RadioButton {
                                id: buttonTraditional
                                checked: settings.useTraditional
                                text: qsTr("Traditional")
                            }
                        }

                    } // optionsCol

                    CheckBox {
                        text: "Use tone colours"
                        onCheckedChanged: settings.toneColorsEnabled = checked
                        Component.onCompleted: {
                            checked = settings.toneColorsEnabled
                        }
                    }
                    Column {
                        clip: true
                        width: parent.width
                        height: buttonRow.height
                        opacity: settings.toneColorsEnabled ? 1 : 0
                        Behavior on opacity {
                            NumberAnimation {
                                duration: 400
                            }
                        }

                        Item {
                            Behavior on x {
                                NumberAnimation {
                                    easing.type: Easing.InOutElastic
                                    duration: 500
                                    easing.amplitude: 1.0; easing.period: 1.5
                                    easing.overshoot: 10
                                }
                            }
                            x: settings.toneColorsEnabled ? 0: -1000
                            width: parent.width
                            height: buttonRow.height
                            Row {
                                id: buttonRow
                                //anchors.centerIn: parent
                                anchors.left: parent.left
                                anchors.leftMargin: UI.MARGIN_XLARGE
                                anchors.right: parent.right
                                anchors.rightMargin: UI.MARGIN_XLARGE

                                spacing: 20
                                ColorButton {
                                    id: button1
                                    text: "1st"
                                    color: settings.tone1Color
                                    onSelectedColorChanged: settings.tone1Color = selectedColor
                                }
                                ColorButton {
                                    id: button2
                                    text: "2nd"
                                    color: settings.tone2Color
                                    onSelectedColorChanged: settings.tone2Color = selectedColor
                                }
                                ColorButton {
                                    id: button3
                                    text: "3rd"
                                    color: settings.tone3Color
                                    onSelectedColorChanged: settings.tone3Color = selectedColor
                                }
                                ColorButton {
                                    id: button4
                                    text: "4th"
                                    color: settings.tone4Color
                                    onSelectedColorChanged: settings.tone4Color = selectedColor
                                }
                                ColorButton {
                                    id: button5
                                    text: "Neutral"
                                    color: settings.tone5Color
                                    onSelectedColorChanged: settings.tone5Color = selectedColor
                                }
                            }
                        }
                    }

                } // settingsCol
            }//contentCol

        } // flickable

    }

}
