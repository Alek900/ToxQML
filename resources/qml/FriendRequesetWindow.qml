import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0

Window{
    signal acceptClicked(var request)
    property var request
    id: root

    width: 400
    height: 200

    ColumnLayout{
        anchors.fill: parent
        anchors.margins: 6
        Label{
            text: "You haved recived a friend request from a user with this id"
        }
        TextField{
            Layout.fillWidth: true

            id: friendrequestid
            text: typeof request == "undefined" ? "" : request.key
            readOnly: true
        }
        Label{
            text: "Here is the accompanying message"
        }
        TextArea{
            Layout.fillHeight: true
            Layout.fillWidth: true
            id: friendrequestmessage
            text: typeof request == "undefined" ? "" :request.message
            readOnly: true
        }
        RowLayout{
            Layout.fillWidth: true
            Button{
                text: "Deny"
                
                onClicked:{
                    root.visible = false
                }
            }
            Item{
                Layout.fillWidth: true
                height: 20
            }
            Button{
                text: "Accept"
                
                onClicked: {
                    acceptClicked(request)
                    root.visible = false
                }
            }
        }
    }
}
