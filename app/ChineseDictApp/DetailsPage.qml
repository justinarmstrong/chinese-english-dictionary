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
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3


import "UIConstants.js" as UI
import "utils.js" as UTILS

Page {
    id: detailsPage
    //anchors.fill: parent

    property int wordsKey
    property string rawEnglish
    property string traditional
    property string simplified
    property string pinyin
    property string toneNums
    property int listRowIndex

    property string alsoPronounced
    property string alsoWritten
    property variant englishDefs: []
    property variant classifiers: []
    property variant componentCharacters: []
    property bool isFavourite

    property variant extraInfo


    onExtraInfoChanged: {
        alsoPronounced = extraInfo.chinese.alsoPronounced
        alsoWritten = extraInfo.chinese.alsoWritten
        englishDefs = extraInfo.english
        traditional = traditional
        simplified = simplified
    }


    Rectangle {
        anchors.fill: parent
        color: UI.DETAILS_BACK_COLOR
    }


    header: ToolBar {
        ToolButton {
            id: backButton
            anchors.left:parent.left
            icon.source : "ic_arrow_back_24px.svg"
            onClicked: pageStack.pop()
        }
        /*
        Label {
            anchors.left: backButton.right
            anchors.right: favouriteButton.left
            elide: Label.ElideRight
            font.pointSize: 28
            text: (settings.useTraditional ? traditional : simplified) + " - DEFINITION"
        }
        */
        ToolButton {
            id: favouriteButton
            anchors.right:parent.right
            icon.source: isFavourite ? "ic_star_24px.svg" : "ic_star_border_24px.svg"
            onClicked: {
                //console.log("toggle " + listRowIndex)
                //var isFavourite = settings.isFavourite(wordsKey)
                if (isFavourite) {
                    //console.log("want to unfavourite " + wordsKey);
                    settings.removeFavourite(wordsKey, listRowIndex);

                } else {
                    //console.log("want to favourite " + wordsKey)
                    settings.addFavourite(wordsKey, traditional, simplified, pinyin, toneNums, rawEnglish)
                }
                isFavourite = settings.isFavourite(wordsKey)
            }
        }

    }


    onWordsKeyChanged: {
        isFavourite = settings.isFavourite(wordsKey)
        //console.log("wordsKey is now " + wordsKey + "  isFavourite? " + isFavourite)
    }


    Flickable {
        id: flickable
        anchors.fill: parent
        //clip: true
        height: parent.height
        flickableDirection: Flickable.VerticalFlick
        contentHeight: UI.PADDING_DOUBLE +
                       contentColumn.height

        Column {
            id: contentColumn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: UI.MARGIN_XLARGE
            spacing: UI.PADDING_DOUBLE
            anchors.rightMargin: UI.MARGIN_XLARGE
            clip: false

            //padding
            Item {
                width: parent.width
                height: UI.PADDING_DOUBLE
            }

            DefBlock {
                title: "English"
                width: parent.width
                EnglishColumn {
                    definitions: englishDefs
                    width: parent.width
                }
                animationDuration: 550
            }


            DefBlock {
                width: parent.width
                title:  (traditional === simplified) ?
                           "Traditional/Simplified" : "Traditional"
                ChineseDef {
                    text: traditional
                    toneNums: detailsPage.toneNums
                    textHeight: UI.FONT_DEFAULT
                    width: parent.width
                }
                SingleLineDef {
                    visible: alsoWritten.length > 0
                    text: UTILS.splitTradSimp(alsoWritten, true)
                    toneNums: detailsPage.toneNums
                    title: "Also written "
                    width: parent.width
                    fontHeight: UI.FONT_LSMALL
                }
                animationDuration: 550
            }


            //only show this if traditional and simplified are not the same
            DefBlock {
                visible: (traditional != simplified)
                title: "Simplified"
                width: parent.width
                ChineseDef {
                    text: simplified
                    toneNums: detailsPage.toneNums
                    textHeight: UI.FONT_DEFAULT
                    width: parent.width
                }
                SingleLineDef {
                    visible: alsoWritten.length > 0
                    text: UTILS.splitTradSimp(alsoWritten, false)
                    toneNums: detailsPage.toneNums
                    title: "Also written "
                    width: parent.width
                    fontHeight: UI.FONT_LSMALL
                }
                animationDuration: 600
            }

            DefBlock {
                title: "Pinyin"
                width: parent.width
                ChineseDef {
                    text: pinyin
                    isHanzi: false
                    toneNums: detailsPage.toneNums
                    textHeight: UI.FONT_DEFAULT
                    width: parent.width
                }
                SingleLineDef {
                    visible: alsoPronounced.length > 0
                    text: alsoPronounced
                    isHanzi: false
                    toneNums: detailsPage.toneNums
                    title: "Also pronounced "
                    width: parent.width
                    fontHeight: UI.FONT_LSMALL
                }
                animationDuration: 650
            }

            DefBlock {
                visible: (classifiers.length > 0)
                title: (classifiers.length > 1) ? "Classifiers" : "Classifier"
                width: parent.width
                DefinitionsColumn {
                    definitions: classifiers
                    width: parent.width

                }
                animationDuration: 600
            }

            //////////////////////////////////////////////////////////////////////////
            //if more than one character, show component characters
            //////////////////////////////////////////////////////////////////////////

            DefBlock {
                visible: componentCharacters.length > 1
                title: "Characters in " + UTILS.toneColorOnCharacters(toneNums, (settings.useTraditional ? traditional : simplified))

                width: parent.width
                DefinitionsColumn {
                    definitions: componentCharacters
                    width: parent.width
                }
                animationDuration: 600
            }

            Button {
                id: searchByClassifierButton
                visible: (rawEnglish.match(/classifier for/) != null)
                width: parent.width
                text: "Find words that use " +
                      (settings.useTraditional ? traditional : simplified) +
                      " as a classifier"
                onClicked: {
                    pageStack.pop()
                    tabBar.currentIndex = 0
                    //FIXME exposing ugly dependency on trad chars here
                    appWindow.searchPage.switchToChineseAndSearch("CL:" + traditional)
                }
            }

        } // contentColumn
        ScrollIndicator.vertical: ScrollIndicator {}
    }


}
