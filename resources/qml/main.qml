import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0

ApplicationWindow {
    property var currentfriend

    id: root
    visible: true
    width: 600
    height: 400

    Connections{
        target: CoreModel

        onFriendRequest: {
            var req = request
            friendrequesetrecivedwindow.request = req
            friendrequesetrecivedwindow.visible = true
        }
    }

    RequestFriendWindow {
        id: newfriendrequestwindow
        onClickedSend: {
            CoreModel.sendFriendrequest(key, message)
        }
    }

    FriendRequesetWindow {
        id: friendrequesetrecivedwindow
        onAcceptClicked: {
            CoreModel.acceptFriendRequest(request)
        }
    }

    Window{
        id: showouruserid
        width: userid.__contentWidth
        height: 50
        minimumHeight: 50
        maximumHeight: 50

        Item{
            anchors.fill: parent
            anchors.margins: 6
            TextField{
                id: userid
                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: parent.left
                anchors.right: parent.right
                text: CoreModel.user.userId
                readOnly: true
            }
        }

    }

    Item {
        id: ouruser
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: friendslist.right
        height: 60

        Image {
            id: useravatar
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 6
            fillMode: Image.PreserveAspectFit
            source: "qrc:/icons/avatar-default.png"
            MouseArea{
                anchors.fill: parent

                onDoubleClicked: {
                    showouruserid.visible = true
                }
            }
        }

        Label {
            id: usernametext
            anchors.left: useravatar.right
            anchors.bottom: useravatar.verticalCenter
            text: CoreModel.user.username == "" ? "New user" : CoreModel.user.username
            font.pointSize: 13

            TextField {
                id: editusername
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                visible: false
                text: parent.text
                onAccepted: {
                    CoreModel.setuserUsername(text)
                    visible = false
                }

                onFocusChanged: {
                    if (focus == false) {
                        visible = false
                    }
                }

                Keys.onPressed: {
                    if (event.key === Qt.Key_Escape) {
                        focus = false
                    }
                }
            }

            MouseArea {
                anchors.fill: parent

                onDoubleClicked: {
                    editusername.focus = true
                    editusername.selectAll()
                    editusername.visible = true
                }
            }
        }

        Label {
            id: userstatusmessage
            anchors.top: usernametext.bottom
            anchors.left: usernametext.left
            text: CoreModel.user.statusNote ? CoreModel.user.statusNote : "Online"
            color: usernametext.color
            font.pointSize: usernametext.font.pointSize - 2
            TextField {
                id: edituserstatusmessage
                anchors.top: parent.top
                anchors.left: parent.left
                visible: false
                text: userstatusmessage.text
                onAccepted: {
                    CoreModel.setuserStatusnote(text)

                    visible = false
                }

                onFocusChanged: {
                    if (focus == false) {
                        visible = false
                    }
                }

                Keys.onPressed: {
                    if (event.key === Qt.Key_Escape) {
                        focus = false
                    }
                }
            }

            MouseArea {
                anchors.fill: parent

                onDoubleClicked: {
                    edituserstatusmessage.focus = true
                    edituserstatusmessage.selectAll()
                    edituserstatusmessage.visible = true
                }
            }
        }

        Image{
            id:connectionstatusicon
            anchors.top: parent.top
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 6
            fillMode: Image.PreserveAspectFit
            source: CoreModel.connected ? "qrc:/icons/user-available.png" : "qrc:/icons/user-offline.png"
        }

    }

    SystemPalette {
        id: palette
        colorGroup: SystemPalette.Active
    }

    SystemPalette {
        id: disabledpalette
        colorGroup: SystemPalette.Disabled
    }

    TableView {
        id: friendslist

        anchors.top: ouruser.bottom
        anchors.bottom: friendstoolbar.top
        anchors.left: parent.left

        width: 230
        headerVisible: false
        model: CoreModel.friends

        ListModel {
            id: statusList
            ListElement {
                iconSource: "qrc:/icons/user-available.png"
            }
            ListElement {
                iconSource: "qrc:/icons/user-away.png"
            }
            ListElement {
                iconSource: "qrc:/icons/user-busy.png"
            }
            ListElement {
                iconSource: "qrc:/icons/user-offline.png"
            }
            ListElement {
                iconSource: "qrc:/icons/dialog-question.png"
            }
        }

        Menu{
            id: friendmenu

            MenuItem{
                text: "Delete"
                onTriggered: {
                    root.currentfriend.deleteMe();
                }
            }

        }

        rowDelegate: Rectangle {
            height: 60
            color: styleData.selected ? palette.highlight : palette.base
        }

        itemDelegate: Item {
            Image {
                id: friendavatar
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.margins: 6
                fillMode: Image.PreserveAspectFit
                source: "qrc:/icons/avatar-default.png"
            }
            Text {
                id: username
                anchors.left: friendavatar.right
                anchors.bottom: friendavatar.verticalCenter
                color: styleData.selected ? palette.highlightedText : palette.windowText
                text: friendslist.model[styleData.row].username
                font.pointSize: 13
            }
            Text {
                id: status
                anchors.top: username.bottom
                anchors.left: username.left
                text: CoreModel.friends[styleData.row].statusNote
                color: username.color
                font.pointSize: username.font.pointSize - 2
            }
            Image {
                anchors.margins: 6
                antialiasing: true
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                source: statusList.get(friendslist.model[styleData.row].status).iconSource
                width: 24
                smooth: true
                fillMode: Image.PreserveAspectFit
            }
        }

        onCurrentRowChanged: {
            currentfriend = model[currentRow]
        }

        MouseArea{
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            onClicked: {
                if (parent.currentRow >= 0)
                {
                    friendmenu.__popup(mouseX + parent.x, mouseY + parent.y, -1)
                }
            }
        }

        TableViewColumn {
        }
    }

    Rectangle{
        anchors.left: friendslist.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        color: Qt.darker(palette.window)
        width: 1
        anchors.margins: -1
    }

    Item {
        id: friendstoolbar
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: friendslist.right
        height: 50

        Rectangle{
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 6
            color: palette.window
            width: 48
            Image{
                anchors.centerIn: parent
                source: "qrc:/icons/contact-new.png"

            }
            MouseArea{
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    parent.border.width = 1
                }
                onExited: {
                    parent.border.width = 0
                }
                onClicked: {
                    newfriendrequestwindow.visible = true
                }
            }
        }
    }

    ChatArea{
        friend: currentfriend
        id: chatviews
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.left: friendslist.right
        anchors.bottom: parent.bottom
    }
}
