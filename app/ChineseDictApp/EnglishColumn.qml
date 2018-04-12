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
import "utils.js" as UTILS

Column {

    width: parent.width
    /*
     * expects a data model like
     * [
     *   { english: "xxx", chinese:[ "t|s", "t|s" ] },
     *   { english: "yyy", chinese:[ ] },
     *   { english: "zzz", chinese:[ "t|s" ] }
     * ]
     */


    property variant definitions;

    Repeater {
        id: repeater
        model: definitions
        width: parent.width
        Item {
            width: repeater.width
            height: edit.height
            Rectangle {
                radius: 4
                smooth: true
                x: defRow.x - 6

                width: 12 + ((definitions.length > 1) ?
                            (bullet.width + edit.paintedWidth) :
                            edit.paintedWidth)

                //width: Math.min((bullet.paintedWidth + edit.paintedWidth), parent.width)+12
                height: edit.height
                color: "#cecbce"
                opacity: clickArea.pressed ? 1 : 0

                Behavior on opacity {
                    NumberAnimation {
                        duration: 300
                    }
                }
            }

            Row {
                id: defRow
                width: parent.width

                TextEdit {
                    id: bullet
                    textFormat: Text.PlainText
                    font.weight: Font.Normal
                    font.pixelSize: (definitions.length > 1) ? UI.FONT_SMALL : UI.FONT_LSMALL
                    text: "• "
                    visible: (definitions.length > 1)
                    color: UI.DEFAULT_TEXT_COLOR
                }
                TextEdit {
                    id: edit
                    clip: false
                    focus: true
                    textFormat: Text.PlainText
                    text: modelData.english
                    font.weight: Font.Normal
                    font.pixelSize: (definitions.length > 1) ?  UI.FONT_SMALL : UI.FONT_LSMALL
                    wrapMode: Text.Wrap
                    width: parent.width - bullet.width
                    color: UI.DEFAULT_TEXT_COLOR

                    readOnly: true

                    MouseArea {
                        id: clickArea
                        width: parent.width
                        height: parent.height
                        onPressAndHold: {
                            if (modelData.inlineChinese.length > 0) {
                                inlineChineseContextMenu.open()
                            } else {
                                copyOnlyContextMenu.open()
                            }
                        }

                    }

                    Menu {
                        id: inlineChineseContextMenu
                        MenuItem {
                            text: "Copy '" + modelData.english + "'"
                            onClicked: {
                                edit.selectAll()
                                edit.copy()
                                showMessage("Copied '" + edit.selectedText + "' to clipboard");
                                edit.deselect()
                            }
                        }
                        Repeater {
                            id: menuRepeater
                            model: modelData.inlineChinese
                            MenuItem {
                                text: "Search for '" + UTILS.splitTradSimp(modelData, settings.useTraditional) + "'"
                                onClicked: {
                                    pageStack.pop()
                                    tabGroup.currentTab = appWindow.searchPage
                                    appWindow.searchPage.switchToChineseAndSearch(UTILS.splitTradSimp(modelData, settings.useTraditional))
                                }
                            }
                        }
                    } // inlineChineseContextMenu

                    Menu {
                        id: copyOnlyContextMenu
                        MenuItem {
                            text: "Copy '" + modelData.english + "'"
                            onClicked: {
                                edit.selectAll()
                                edit.copy()
                                showMessage("Copied '" + edit.selectedText + "' to clipboard");
                                edit.deselect()
                            }
                        }
                    } // copyOnlyContextMenu

                } //TextEdit
            } // Row

            x:-1000
            Component.onCompleted: {
                x = 0
            }
            onVisibleChanged: {
                if (!visible) x = -1000;
                else x = 0;
            }
            Behavior on x {
                NumberAnimation {
                    easing.type: Easing.InOutElastic
                    duration: 500 + (index * 80)
                    easing.amplitude: 1.0; easing.period: 1.5
                    easing.overshoot: 10
                }
            }

        } //Item

    }
}
