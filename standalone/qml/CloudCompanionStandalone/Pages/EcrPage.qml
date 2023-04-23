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

    Component {
        id: sectionHeading
        Rectangle {
            width: ecrList.width
            height: childrenRect.height
            color: "lightsteelblue"

            Text {
                text: section
                font.bold: true
                font.pixelSize: 14
                //font.pixelSize: 64 //Mobile
            }
        }
    }

    ListView {
        id: ecrList
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.topMargin: 5
        anchors.leftMargin: 5
        width: parent.width / 2
        height: parent.height
//        width: 400 //Mobile
//        height: 400 //Mobile
        model: ecr_list_model
        focus: true
        activeFocusOnTab: true

        onCurrentIndexChanged: {
            if(ecr_list_model != null)
                ecr_list_model.selectedIndexChanged(currentIndex);
        }

        delegate: Rectangle {
                      height: 20; width: ecrList.width
                      border.color: "#CCCCCC"
                      color: "transparent"

                      Column {
                          Text { text: ecr_tag; font.pixelSize: 12; }
                          //Text { text: ecr_tag; font.pixelSize: 62; } //Mobile
                      }

                      MouseArea {
                          anchors.fill: parent
                          onClicked: ecrList.currentIndex = index
                      }
                  }
        highlight: Rectangle {
            width: ecrList.width
            color: "#eeeeee"
        }
        section.property: "ecr_name"
        section.criteria: ViewSection.FullString
        section.delegate: sectionHeading
    }

    Button {
        id: btnRefreshEcr
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: ecrList.right
        anchors.topMargin: 5
        anchors.leftMargin: 5
        text: "Refresh ECR"

        onClicked: ecr_list_model.btnRefreshEcrClick()
    }

    Connections {
        target: ecr_list_model
        function onShowMessage(text, timeout){
         Qaterial.SnackbarManager.snackBarLoader.show(
          {
            text: text,
            timeout: timeout,
            canBeKilled: true
          });
        }
    }

  } // Pane
} // Page
