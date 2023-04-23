import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import Qaterial as Qaterial

Qaterial.Page
{
  id: _root
  property bool enabled: true

  Qaterial.Pane
  {
    padding: 0

    elevation: 20
    anchors.centerIn: parent
    width: parent.width - 30
    height: parent.height - 40
    implicitHeight: height

    Column
    {
        id: _column
        width: Math.min(parent.width, 500)
        //spacing: Qaterial.Style.card.verticalPadding
        anchors.horizontalCenter: parent.horizontalCenter

        Settings {
            category: "settings/credentials"
            property alias type: provider_combo.currentIndex
            property alias accessKeyId: _accessKeyId.text
            property alias secretKey: _secretKey.text
            property alias sessionToken: _sessionToken.text
            property alias samlUsername: _samlUsername.text
            property alias samlPassword: _samlPassword.text
            property alias sshPrivateKey: _sshPrivateKeyPath.text
            property alias samlRoleArn: _samlRoleArn.text
            property alias samlPrincipalArn: _samlPrincipalArn.text
        }

        Row
        {
            Qaterial.Label
            {
                id: provider_label
                width: _column.width*0.3
                height: parent.height
                horizontalAlignment: Qt.AlignHLeft
                verticalAlignment: Qt.AlignVCenter
                text: "Authentication provider: "
            } // Label

            Qaterial.ComboBox
            {
                id: provider_combo
                width: _column.width*0.6
                model: ["Default", "Access Key ID & Secret Key", "Security Token Service (STS)", "SAML (Okta)"]
            } // ComboBox
        } // Row
        Row
        {
            Qaterial.TextField
            {
              id: _accessKeyId
              width: _column.width*0.4
              title: "Access Key ID"
              visible: provider_combo.currentIndex !== 3
            } // TextField

            Qaterial.TextField
            {
              id: _secretKey
              width: _column.width*0.4
              title: "Secret Key"
              echoMode: TextInput.Password
              trailingContent: Qaterial.TextFieldPasswordButton { }
              visible: provider_combo.currentIndex !== 3
            } // TextField

            Qaterial.TextField
            {
              id: _sessionToken
              width: _column.width
              title: "Session Token"
              echoMode: TextInput.Password
              trailingContent: Qaterial.TextFieldPasswordButton { }
              visible: provider_combo.currentIndex !== 3
            } // TextField

            Qaterial.TextField
            {
              id: _samlUsername
              width: _column.width*0.4
              title: "SAML Username"
              visible: provider_combo.currentIndex === 3
            } // TextField

            Qaterial.TextField
            {
              id: _samlPassword
              width: _column.width*0.4
              title: "SAML Password"
              echoMode: TextInput.Password
              trailingContent: Qaterial.TextFieldPasswordButton { }
              visible: provider_combo.currentIndex === 3
            } // TextField
        }

        Row
        {
            Qaterial.TextField
            {
              id: _samlRoleArn
              width: _column.width
              title: "Role ARN"
              visible: provider_combo.currentIndex === 3
            } // TextField
        }

        Row
        {
            Qaterial.TextField
            {
              id: _samlPrincipalArn
              width: _column.width
              title: "Principal ARN"
              visible: provider_combo.currentIndex === 3
            } // TextField

        } // Row
        Row
        {
            Qaterial.TextField
            {
              id: _sshPrivateKeyPath
              width: _column.width*0.8
              title: "SSH private key path"
            } // TextField

            Qaterial.SquareButton
            {
              icon.source: "qrc:/CloudCompanionStandalone/folder-open.svg"
              checkable : true
              onClicked: fileDialog.open()
            } // SquareButton

            FileDialog {
                id: fileDialog
                title: "Select private key for SSH login"
                visible: false
                nameFilters: [ "PEM file (*.pem)" ]

                onAccepted: {
                    fileDialog.close()
                    _sshPrivateKeyPath.text = fileDialog.fileUrl
                }
            }
        } // Row
    } // Column
  } // Pane
} // Page
