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

    Window{
        id: newfriendrequestwindow
        width: 400
        height: 200
        modality: Qt.WindowModal

        ColumnLayout{
            anchors.fill: parent
            anchors.margins: 8
            spacing: 8
            TextField{
                id: newfriendid
                Layout.fillWidth: true
                placeholderText: "Friend id"
            }
            Label{
                text: "Message you want to send with the request"
            }

            TextArea{
                id: newfriendmessage
                Layout.fillHeight: true
                Layout.fillWidth: true
            }
            RowLayout{
                Button{
                    text: "Cancel"

                    onClicked: {
                        newfriendrequestwindow.visible = false
                    }
                }
                Item{
                    Layout.fillWidth: true
                    height: 20

                }
                Button{
                    text: "Send request"
                    onClicked: {
                        CoreModel.sendFriendrequest(newfriendid.text, newfriendmessage.text)
                        newfriendrequestwindow.visible = false
                        newfriendid.text = ""
                        newfriendmessage = ""
                    }
                }
            }
        }

    }

    Window{
        property var request
        id: friendrequesetrecivedwindow
        ColumnLayout{
            Label{
                text: "You haved recived a friend request from a user with this id"
            }
            TextField{
                id: friendrequestid
                text: request ? "" : request.key
                readOnly: true
            }
            Label{
                text: "Here is the accompanying message"
            }
            TextArea{
                id: friendrequestmessage
                text: request ? "" :request.message
                readOnly: true
            }
            RowLayout{
                Button{
                    text: "Deny"

                    onClicked:{
                        friendrequesetrecivedwindow.visible = false
                    }
                }
                Item{
                    Layout.fillWidth: true
                    height: 20
                }
                Button{
                    text: "Accept"

                    onClicked: {
                        friendrequesetrecivedwindow.visible = false
                        CoreModel.acceptFriendRequest(request)
                    }
                }
            }
        }
    }

    Item {
        id: ouruser

        anchors.left: parent.left
        anchors.top: parent.top
        height: 60

        Image {
            id: useravatar
            anchors.left: parent.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: 6
            fillMode: Image.PreserveAspectFit
            source: "qrc:/icons/avatar-default.png"
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
                iconSource: "qrc:/icons/user-invisible.png"
            }
            ListElement {
                iconSource: "qrc:/icons/dialog-question.png"
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
            MouseArea{
                anchors.fill: parent
                onDoubleClicked: {
                    friendslist.currentRow = styleData.row
                }
            }
        }

        onCurrentRowChanged: {
            currentfriend = model[currentRow]
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

    Rectangle{
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.left: friendslist.right
        anchors.bottom: parent.bottom

        color: palette.base
        clip: true

        Rectangle{
            id: chatviewport
            anchors.margins: 6
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: textviewport.top
            Flickable{
                id: chatscroller
                anchors.fill: parent
                contentHeight: chatmessages.contentHeight + chatmessages.font.pixelSize + 6
                boundsBehavior: Flickable.StopAtBounds

                TextEdit{
                    id: chatmessages
                    anchors.fill: parent
                    color: palette.text
                    selectionColor: palette.highlightedText

                    text: currentfriend.chatlog

                    onTextChanged: {
                        if (contentHeight > chatviewport.height)
                        {
                            chatscroller.contentY = contentHeight - chatviewport.height
                        } else {
                            chatscroller.contentY = 0
                        }
                    }

                }

            }

        }
        Rectangle{
            id: textviewport
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 4

            antialiasing: true
            radius: 3
            border.width: 1
            border.color: disabledpalette.dark

            height: 50
            clip: true
            Flickable{
                id: textscroller
                anchors.fill: parent
                anchors.margins: 6
                contentHeight: chatinput.contentHeight + chatinput.font.pixelSize + 6
                boundsBehavior: Flickable.StopAtBounds

                TextEdit{
                    id: chatinput
                    anchors.fill: parent
                    color: palette.text
                    selectionColor: palette.highlightedText

                    onCursorPositionChanged: {
                        if (cursorRectangle.y >= textscroller.contentY + textviewport.height - 1.5*cursorRectangle.height - 6)
                        {
                            textscroller.contentY = cursorRectangle.y - textviewport.height + cursorRectangle.height*1.5
                        } else if (cursorRectangle.y < textscroller.contentY) {
                            textscroller.contentY = cursorRectangle.y
                        }
                    }

                    Keys.onPressed: {
                        if (event.key == Qt.Key_Enter || event.key == Qt.Key_Return){
                            if(!(event.modifiers & Qt.ShiftModifier))
                            {
                                currentfriend.sendMessage(text)
                                text = ""
                                event.accepted = true
                            }
                        }
                    }

                    Text{
                        text: qsTr("Type your message here...")
                        color: disabledpalette.text
                        visible: !parent.focus && !parent.text.length
                    }
                }

            }

        }
    }
}
