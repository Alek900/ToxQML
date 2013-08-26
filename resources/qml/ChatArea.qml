import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.0

Item{
    property var friend

    SystemPalette {
        id: palette2
        colorGroup: SystemPalette.Active
    }


    Rectangle{
        clip: true
        id: chatviewport
        anchors.margins: 4
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: textviewport.top
        border.width: 1
        border.color: disabledpalette.highlight
        radius: 3
        Flickable{
            id: chatscroller
            anchors.fill: parent
            anchors.margins: 6
            contentHeight: chatmessages.contentHeight + chatmessages.font.pixelSize + 6
            boundsBehavior: Flickable.StopAtBounds
            
            TextEdit{
                id: chatmessages
                anchors.fill: parent
                color: palette.text
                selectionColor: palette2.highlight
                wrapMode: TextEdit.WordWrap
                selectByMouse: true
                selectedTextColor: palette2.highlightedText

                text: typeof friend == "undefined" ? "" : friend.chatlog
                
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
                color: palette2.text
                selectionColor: palette2.highlight
                selectedTextColor: palette2.highlightedText
                selectByMouse: true
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
                            friend.sendMessage(text)
                            text = ""
                            event.accepted = true
                        }
                    } else if (event.key == Qt.Key_Escape) {
                        text = ""
                        focus = false
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
