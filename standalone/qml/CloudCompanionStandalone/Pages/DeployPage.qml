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

    Qaterial.TextField
    {
      id: _targetIp
      width: parent.width*0.25
      anchors.verticalCenter: parent.verticalCenter
      anchors.horizontalCenter: parent.horizontalCenter
      title: "Machine IP"
    } // TextField

   Button {
        id: btnDeployEcr
        anchors.top: _targetIp.top
        anchors.left: _targetIp.right
        anchors.leftMargin: 5
        anchors.topMargin: 5
        text: "Deploy Image"

        onClicked: ssh_handler.deployDockerImage(_targetIp.text)
    }

    Qaterial.SnackbarLoader
    {
        id: _snackBarLoader
    }

    Connections {
        target: instances_table_model
        function onEc2Selected(ip){
            _targetIp.text = ip;
        }
    }

  } // Pane
} // Page
