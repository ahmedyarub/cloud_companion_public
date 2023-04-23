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
      id: txtGitUrl
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.bottom: txtImageName.top
      anchors.bottomMargin: 5
      width: parent.width * 2 / 3
      title: "Git repository URL"
    } // TextField

    Qaterial.TextField
    {
      id: txtImageName
      text: "672775987525.dkr.ecr.us-east-1.amazonaws.com/hello-world:latest"
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.bottom: txtDockerRegistryUrl.top
      anchors.bottomMargin: 5
      width: parent.width * 2 / 3
      title: "Docker image name"
    } // TextField

    Qaterial.TextField
    {
      id: txtDockerRegistryUrl
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.bottom: btnBuildImageFromFile.top
      anchors.bottomMargin: 5
      width: parent.width * 2 / 3
      title: "Docker registry full URL"
    } // TextField

    Button {
        id: btnBuildImageFromFile
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 5
        anchors.leftMargin: 5
        text: "Build Image from File"

        onClicked: fileDialog.open()
    }

    Button {
        id: btnBuildImageFromGit
        anchors.verticalCenter: parent.verticalCenter
        anchors.right: btnBuildImageFromFile.left
        anchors.topMargin: 5
        anchors.leftMargin: 5
        text: "Build Image from Git"

        onClicked: ecr_list_model.btnBuildImageFromGit(txtGitUrl.text, txtImageName.text)
    }

    Button {
        id: btnPushImage
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: btnBuildImageFromFile.right
        anchors.topMargin: 5
        anchors.leftMargin: 5
        text: "Push Image"

        onClicked: ecr_list_model.btnPushImage(txtImageName.text, txtDockerRegistryUrl)
    }

    FileDialog {
        id: fileDialog
        title: "Select Dockerfile to build"
        visible: false
        nameFilters: [ "Tar Archived Dockerfile (*.tar)" ]

        onAccepted: {
            console.log("You chose: " + fileDialog.fileUrl)
            fileDialog.close()
            ecr_list_model.btnBuildImageFromFile(fileDialog.fileUrl)
        }
    }
  } // Pane
} // Page
