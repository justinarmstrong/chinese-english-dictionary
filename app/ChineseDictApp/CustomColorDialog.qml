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

Dialog {
    id: dialog
    property string selectedColor
    property string text

    width: parent.width
    height: parent.height
    modal: true
    standardButtons: Dialog.Cancel
    parent: ApplicationWindow.overlay
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    title: dialog.text
    Column {
        spacing: 0
        width: parent.width

        Grid {
            id: grid
            columns: 5
            width: parent.width
            spacing: 5
            property int blockSide: (parent.width/columns)-spacing
            Repeater {
                Rectangle {
                    color: modelData
                    width: grid.blockSide
                    height: grid.blockSide
                    border.color: "black"
                    border.width: (dialog.selectedColor === modelData) ? 4 : 0

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            dialog.selectedColor = modelData
                            dialog.accept()
                        }
                    }
                }

                model: ListModel {
                    //green
                    ListElement {
                        color: "#94d363"
                    }
                    ListElement {
                        color: "#61c220"
                    }
                    ListElement {
                        color: "#58ac2c"
                    }
                    ListElement {
                        color: "#218c17"
                    }
                    ListElement {
                        color: "#24722c"
                    }

                    //blue
                    ListElement {
                        color: "#68c0dd"
                    }
                    ListElement {
                        color: "#309add"
                    }
                    ListElement {
                        color: "#287ccd"
                    }
                    ListElement {
                        color: "#1764cb"
                    }
                    ListElement {
                        color: "#103b83"
                    }


                    //purple
                    ListElement {
                        color: "#bc80dd"
                    }
                    ListElement {
                        color: "#996fd3"
                    }
                    ListElement {
                        color: "#b24fdd"
                    }
                    ListElement {
                        color: "#8432ad"
                    }
                    ListElement {
                        color: "#6a2a78"
                    }

                    //red
                    ListElement {
                        color: "#ff8080"
                    }
                    ListElement {
                        color: "#ff4a40"
                    }
                    ListElement {
                        color: "#ff0000"
                    }
                    ListElement {
                        color: "#bd0000"
                    }
                    ListElement {
                        color: "#800000"
                    }

                    //yellow-orange
                    ListElement {
                        color: "#fff880"
                    }
                    ListElement {
                        color: "#fff000"
                    }
                    ListElement {
                        color: "#ffc616"
                    }
                    ListElement {
                        color: "#ff8a21"
                    }
                    ListElement {
                        color: "#ff542c"
                    }

                    //white-black
                    ListElement {
                        color: "#bdbcbb"
                    }
                    ListElement {
                        color: "#8a867e"
                    }
                    ListElement {
                        color: "#4e4c4e"
                    }
                    ListElement {
                        color: "#323332"
                    }
                    ListElement {
                        color: "#000000"
                    }
                }
            }
        }
    }
}

