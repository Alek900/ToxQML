import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0
import tox.friends 1.0

ApplicationWindow {
    property var currentFriend : false
    id:root
    visible: true
    width: 600
    height: 400

    toolBar: ToolBar{
        RowLayout{
            anchors.fill: parent
            TextField{
                Layout.preferredWidth: 520
                text: CoreModel.user.userId
                readOnly: true
            }
        }
    }
    Connections {
        target: CoreModel

        onFriendRequest:{
            var req = request
            friendRequestDialog.request = request
            friendRequestDialog.visible = true
        }
    }

    Window{
        id: addfriendwindow
        visible: false
        modality: Qt.WindowModal
        width: 300
        height: 200
        ColumnLayout{
            anchors.fill: parent
            TextField{
                Layout.fillWidth: true
                id: addfriendid
                placeholderText: "user ID"
            }
            TextArea{
                Layout.fillWidth: true
                Layout.fillHeight: true
                id: addfriendmessage
                text: "Hello, this is "
            }
            RowLayout{
                Layout.fillWidth: true
                Button{
                    text: "Cancel"
                    onClicked: {
                        addfriendmessage.text = "Hello, this is"
                        addfriendwindow.visible = false
                    }
                }
                Item{
                    Layout.fillWidth: true
                }

                Button{
                    text: "Send request"

                    onClicked: {
                        CoreModel.sendFriendrequest(addfriendid.text, addfriendmessage.text);
                       addfriendmessage.text = "Hello, this is"
                        addfriendid.text = ""
                        addfriendwindow.visible = false
                    }
                }
            }
        }
    }

    Window{
        property var request

        id: friendRequestDialog
        title: "Friend request"

        width: layoutColumn.implicitWidth + layoutColumn.anchors.margins*2
        height : layoutColumn.implicitHeight+ layoutColumn.anchors.margins*2
        modality: Qt.WindowModal
        ColumnLayout{
            id: layoutColumn
            anchors.fill: parent
            anchors.margins: 12
            Label{
                Layout.alignment: Qt.AlignHCenter
                text: "User with the ID: "
            }

            TextField{
                Layout.alignment: Qt.AlignHCenter
                id: fieldId
                text: friendRequestDialog.request.key
            }

            TextArea{
                Layout.alignment: Qt.AlignHCenter
                id: fieldMessage
                text: friendRequestDialog.request.message
            }
            Label{
                Layout.alignment: Qt.AlignHCenter
                text: "Do you accept"
            }
            RowLayout
            {
                Layout.alignment: Qt.AlignHCenter
                Button{
                    text: "Yes"
                    onClicked: {
                        CoreModel.acceptFriendRequest(friendRequestDialog.request)
                        friendRequestDialog.visible = false
                    }
                }
                Button{
                    text: "No"
                    onClicked :{
                        friendRequestDialog.visible = false
                    }
                }
            }


        }
    }

    SplitView {
        anchors.fill: parent
        ColumnLayout
        {
            clip: true
            Row{
                Layout.fillWidth: true
                spacing: 10
                height: 60
                Image{
                    height: 64
                    width:height

                }
                ColumnLayout{
                    Layout.fillHeight: true
                    Text{
                        id: name
                        text: nameedit.text == "" ? "username" : nameedit.text
                        MouseArea{
                            anchors.fill: parent
                            onDoubleClicked: {
                                name.visible = false
                                nameedit.visible = true

                            }
                        }
                    }
                    TextField{
                        id:nameedit
                        visible: false
                        text: CoreModel.user.username
                        onAccepted: {
                            CoreModel.setuserUsername(text)
                            nameedit.visible = false
                            name.visible = true
                        }
                    }

                    Text{
                        id: note
                        font.pointSize: name.font.pointSize - 2
                        text: noteedit.text
                        MouseArea{
                            anchors.fill: parent

                            onDoubleClicked: {
                                note.visible = false
                                noteedit.visible = true

                            }
                        }
                    }

                    TextField{
                        id:noteedit
                        visible: false
                        text: "My long ass-status"
                        onAccepted: {
                            CoreModel.setuserStatusnote(text)
                            noteedit.visible = false
                            note.visible = true
                        }
                    }
                }

            }

            TableView{
                id: friendsview
                Layout.fillHeight: true
                Layout.fillWidth: true

                model: CoreModel.friends

                TableViewColumn{
                    title: "Status"
                    role: "status"

                }
                TableViewColumn{
                    title: "Name"
                    role: "username"

                }
                TableViewColumn{
                    title: "UserID"
                    role: "userId"

                }
                onCurrentRowChanged: {
                    currentFriend = CoreModel.friends[currentRow]
                    console.log("Selction changed")
                }
            }
            TableView{
                id: requestview
                Layout.fillHeight: true
                Layout.fillWidth: true
                visible: false
                model: CoreModel.requests

                TableViewColumn{
                    title: "Requests"
                    role: "key"
                }
            }
            RowLayout{
                Layout.minimumWidth: 0
                Button{
                    id: friendsbutton
                    text:"Friends"
                    visible: false
                    onClicked: {
                        friendsbutton.visible = false
                        requestview.visible = false
                        acceptrequest.visible = false

                        requestbutton.visible = true
                        friendsview.visible = true
                        addfriend.visible = true
                    }
                }
                Button{
                    id: requestbutton
                    text: "Requests"

                    onClicked: {
                        requestbutton.visible = false
                        friendsview.visible = false
                        addfriend.visible = false

                        friendsbutton.visible = true
                        requestview.visible = true
                        acceptrequest.visible = true
                    }
                }
                Button{
                    id: addfriend
                    text: "add friend"
                    onClicked: {
                        addfriendwindow.visible = true
                    }
                }

                Button{
                    id:acceptrequest
                    text: "accept request"
                    visible: false

                    onClicked: {
                        var request = requestview.model[requestview.currentRow]
                        CoreModel.acceptFriendRequest(request)
                    }
                }

            }
        }

        ColumnLayout {
            RowLayout{
                Label{
                    text: currentFriend.username
                    Layout.fillWidth: true
                }
                Item{
                    Layout.fillWidth: true
                }
                Button{
                    text: "open in new window"
                    visible: currentFriend
                }
            }
            TextArea{
                id: chatlogarea
                Layout.fillHeight: true
                Layout.fillWidth: true
                text: currentFriend.chatlog
            }

            TextField{
                id: chatfield
                Layout.fillWidth: true
                placeholderText: "Send a message"
                onAccepted: {
                    currentFriend.sendMessage(chatfield.text)
                    chatfield.text = ""
                }
            }

        }
    }
}
