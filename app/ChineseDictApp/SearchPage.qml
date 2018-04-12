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
import QtQuick.Extras 1.4

import "UIConstants.js" as UI
import "utils.js" as UTILS

Pane {

    property bool searchInProgress;
    padding: 0

    function switchToChineseAndSearch(query) {
        searchByChineseSwitch.checked = true
        searchBox.text = query
    }
    clip: true

    Item {
        id: titleArea
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        height:48
        anchors.topMargin: 0
        anchors.leftMargin: 10
        anchors.rightMargin: 10
        anchors.bottomMargin: 0
        TextField {
            id: searchBox
            property bool searchByChinese: settings.searchByChinese
            //height:48
            //topPadding:0
            //bottomPadding:0
            rightPadding: clearButton.width+20
            anchors.left: parent.left
            anchors.leftMargin: 0
            anchors.right: searchByChineseSwitch.left
            anchors.rightMargin: 10
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.top: parent.top

            //font.capitalization: Font.AllLowercase
            renderType: Text.NativeRendering

            placeholderText: settings.searchByChinese ? "Enter 简体/繁體 or Pinyin" : "Enter English"
            //only use predictive text if entering english
            inputMethodHints: Qt.ImhNoAutoUppercase /*|
                              (settings.searchByChinese ? Qt.ImhNoPredictiveText : 0)*/
            /*anchors {
                    left: parent.left
                    //leftMargin: UI.MARGIN_XLARGE; rightMargin: UI.MARGIN_XLARGE
                }*/

            Keys.onPressed: {
                //console.log("key pressed: " + event.key)
            }

            function doSearch() {
                if (settings.searchByChinese) {
                    //console.log("searching chinese")
                    matchChineseAsync(text)
                } else {
                    matchEnglishAsync(text)
                }
            }
            onTextChanged: {
                //console.log("text changed:" + text)
                doSearch();
            }

            onSearchByChineseChanged: {
                //console.log("switched languages")
                doSearch();
            }

            //Clear Image on the search box

            Image {
                id: clearButton
                height:24
                width: 24
                anchors.right: parent.right
                anchors.rightMargin: (parent.height - height) / 2
                //anchors.verticalCenter: parent.verticalCenter
                anchors.top: parent.top
                anchors.topMargin: 10
                //anchors.bottom: parent.bottom

                source: (searchBox.text.length > 0) ?
                            "ic_cancel_48px.svg" :
                            "ic_search_48px.svg"

                MouseArea {
                    id: mouseArea
                    anchors.fill: parent
                    onClicked: {
                        //inputContext.reset();
                        searchBox.text = "";
                        searchBox.forceActiveFocus()
                    }
                }
                opacity: mouseArea.pressed && searchBox.text.length > 0 ? 0.5 : 1.0
            }

        } // TextField
        Button {
            id: searchByChineseSwitch
            checkable: true
            //text: checked ? "中文->Eng" : "Eng->中文"
            //font.pointSize: 10
            display: AbstractButton.TextOnly
            anchors.top: parent.top
            anchors.topMargin: 0
            anchors.bottom: parent.bottom //parent.bottom
            anchors.bottomMargin:  8 //FIXME need an 8 for Material, 0 for other themes
            anchors.right: parent.right
            anchors.rightMargin: 0
            Component.onCompleted: checked = settings.searchByChinese
            onCheckedChanged: {
                settings.searchByChinese = checked
            }
            contentItem: Text {
                text: searchByChineseSwitch.checked ? "中文→Eng" : "Eng→中文"
                //color: "black"
                verticalAlignment: Text.AlignVCenter
                //anchors.verticalCenter: parent.verticalCenter
            }

            background: Rectangle {
                border.width: parent.hovered ? 2 : 1
                //anchors.verticalCenter: parent.verticalCenter
                anchors.fill: parent
                color: parent.pressed ? parent.palette.dark : parent.hovered ? parent.palette.button : parent.palette.base
            }
        }

    }

    Item {
        id: mainCol
        anchors.top: titleArea.bottom
        anchors.topMargin: 0
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        clip: true
/*
        Separator {
            id: sep
            width: parent.width
        }
*/

        Label {
            visible: (searchResults.count === 0) &&
                     (searchBox.text.length > 0) &&
                     (!searchInProgress)
            text: "No matches"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            height: visible?
                        parent.height : 0
            font.weight: Font.Normal
            font.pixelSize: UI.FONT_MASSIVE
            fontSizeMode: Text.Fit
            color: UI.COLOR_SECONDARY_FOREGROUND
        }


        Item {
            visible: (searchResults.count === 0) && searchInProgress
            //width: parent.width
            //height: visible? parent.height - (titleArea.y + titleArea.height) : 0
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            BusyIndicator {
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                running:  true
            }
        }

        ListView {
            visible: searchResults.count > 0
            id: listView
            clip: true
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            //height: parent.height - (titleArea.y + titleArea.height)
            spacing: 0
            model: searchResults
            delegate:  BigListDelegate { swipe.enabled:false }

            ScrollIndicator.vertical: ScrollIndicator { }
        }

    }

}

