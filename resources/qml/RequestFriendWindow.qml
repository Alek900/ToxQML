import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0

Window{
    signal clickedSend(string key, string message)
    id: root
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
                    root.visible = false
                }
            }
            Item{
                Layout.fillWidth: true
                height: 20
                
            }
            Button{
                text: "Send request"
                onClicked: {
                    clickedSend(newfriendid.text, newfriendmessage.text)
                    root.visible = false
                    newfriendid.text = ""
                    newfriendmessage.text = ""
                }
            }
        }
    }
    
}
