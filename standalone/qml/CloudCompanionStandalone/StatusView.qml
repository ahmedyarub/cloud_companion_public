import QtQuick
import QtQuick.Controls
import Qaterial 1.0 as Qaterial

Rectangle {
    id: root

    property bool error: errorString
    property string errorString
    property string file
    property string logString

    function append(text) {
        _errorTextArea.append(text);
    }
    function clear() {
        _errorTextArea.clear();
    }

    color: Qaterial.Style.theme === Qaterial.Style.Theme.Dark ? "#24262A" : "white"
    implicitHeight: 100
    implicitWidth: 500

    Flickable {
        id: _flick

        clip: true
        contentHeight: _errorTextArea.height
        contentWidth: width
        height: root.height - _errorRect.height
        width: parent.width

        ScrollBar.horizontal: Qaterial.ScrollBar {
        }
        ScrollBar.vertical: Qaterial.ScrollBar {
        }

        TextEdit {
            id: _errorTextArea

            cursorVisible: true

            //color: Qaterial.Style.textTypeToColor(Qaterial.Style.TextType.Body1)
            font: Qaterial.Style.textTheme.body2
            padding: Qaterial.Style.card.horizontalPadding
            readOnly: true
            selectByKeyboard: bool
            selectByMouse: true
            selectionColor: Qaterial.Style.accentColor
            text: root.logString
            textFormat: TextEdit.RichText
            width: parent.width
            wrapMode: Text.NoWrap

            //font.family: Qaterial.Style.textTypeToFontFamily(Qaterial.Style.TextType.Body1)
            //font.styleName: Qaterial.Style.textTypeToStyleName(Qaterial.Style.TextType.Body1)
            //font.capitalization: Qaterial.Style.fontCapitalization(Qaterial.Style.TextType.Body1)
            //font.letterSpacing: Qaterial.Style.textTypeToLetterSpacing(Qaterial.Style.TextType.Body1)
            //font.pixelSize: Qaterial.Style.textTypeToPixelSize(Qaterial.Style.TextType.Body1)
            onTextChanged: function () {
                cursorPosition = text.length - 1;
                if (height > _flick.height)
                    _flick.contentY = height - _flick.height;
            }
        }
    }
    Rectangle {
        id: _errorRect

        color: root.error ? "#f44336" : "#4CAF50"
        height: 32
        width: parent.width
        y: root.height - height

        Qaterial.Label {
            anchors.left: parent.left
            anchors.leftMargin: Qaterial.Style.card.horizontalPadding
            anchors.right: _version.left
            anchors.rightMargin: Qaterial.Style.card.horizontalPadding
            anchors.verticalCenter: parent.verticalCenter
            elide: Text.ElideRight
            horizontalAlignment: Text.AlignLeft
            text: root.error ? `Fail to load ${root.file}` : ``
            verticalAlignment: Text.AlignVCenter
            width: 200
        } // Qaterial.Label
        Qaterial.Label {
            id: _version

            anchors.right: parent.right
            anchors.rightMargin: Qaterial.Style.card.horizontalPadding
            anchors.verticalCenter: parent.verticalCenter
            font: Qaterial.Style.textTheme.caption
            text: Qaterial.Version.readable
        }
    }
}
