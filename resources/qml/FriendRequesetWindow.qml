import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0

Window{
    signal acceptClicked(var request)
    property var request
    id: root
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
