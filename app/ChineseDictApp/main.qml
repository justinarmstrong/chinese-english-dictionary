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

ApplicationWindow {
    width: 640
    height: 800

    visible: true
    id: appWindow


    property alias searchPage: searchPage
    property alias pageStack: pageStack


    //signals implemented by DictDb, in other thread
    signal matchEnglishAsync(string search)
    signal matchChineseAsync(string search)

    signal requestDetailsAsync(int wordsKey)

    function onSearchInProgressChanged(inProgress) {
        searchPage.searchInProgress = inProgress;
    }

    function onExtraInfoChanged(extraInfoJson) {
        detailsPage.extraInfo = JSON.parse(extraInfoJson)
    }

    function onClassifiersChanged(classifiersJson) {
        detailsPage.classifiers =  JSON.parse(classifiersJson)
    }

    function onComponentCharactersChanged(componentsJson) {
        detailsPage.componentCharacters = JSON.parse(componentsJson)
    }


    StackView {
        id: pageStack
        initialItem: mainPage
        anchors.fill: parent
        anchors.margins: 0
        clip:true

        Page {
            id: mainPage
            //anchors.fill: parent
            anchors.margins: 0
            SwipeView {
                interactive: false
                currentIndex: tabBar.currentIndex
                anchors.margins: 0
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                id: swipeView

                SearchPage {
                    id: searchPage
                }

                FavouritesPage {
                    id: favouritesPage
                }

                SettingsPage {
                    id: settingsPage
                }

                onCurrentIndexChanged: {
                    tabBar.currentIndex = currentIndex
                }
            } // SwipeView
            header: TabBar {
                id: tabBar
                currentIndex: swipeView.currentIndex
                TabButton {
                    text: "Search"
                    //icon.source: "ic_search_48px.svg"
                }
                TabButton {
                    text: "Saved"
                    //icon.source: "ic_star_24px.svg"
                }
                TabButton {
                    text: "Settings"
                    //icon.source: "ic_settings_48px.svg"
                }
                onCurrentIndexChanged: {
                    swipeView.currentIndex = currentIndex
                 }
            }
        } // Page


        DetailsPage {
            id: detailsPage
            visible: false
        }
/*
        Keys.onBackPressed: {
            console.log("back pressed")
            pageStack.pop()
        }
*/


    }


    onClosing: {
        if(pageStack.depth > 1){
            close.accepted = false
            pageStack.pop();
        } else {
            return;
        }
    }


}
