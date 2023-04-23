import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt.labs.platform

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
    width: parent.width - 5
    height: parent.height - 40
    implicitHeight: height

    TableView {
        id: tblInstances

        columnWidthProvider: function (column) {
            return 180
            //return 440 //Mobile
        }

        rowHeightProvider: function (column) {
            return 60
            //return 160 //Mobile
        }
        topMargin: columnsHeader.implicitHeight
        anchors.left: parent.left
        anchors.leftMargin: 5
        width: parent.width
        height: parent.height / 2
        clip: true

        model: instances_table_model

        delegate: Component {
            Loader {
                sourceComponent: {
                    if (model === undefined || model === null
                            || parent === null) {
                        return
                    }

                    var component_qml = "import QtQuick 2.15; import QtQuick.Controls 2.15; Component { Rectangle { border.color: 'black';"

                    switch(column){
                        case 0:
                            component_qml += " Text{padding: 10; text: ec2_instance_id}";
                            break;
                        case 1:
                            component_qml += " Text{padding: 10; text: ec2_ami}";
                            break;
                        case 2:
                            component_qml += " Text{padding: 10; text: ec2_instance_type}";
                            break;
                        case 3:
                            component_qml += " Text{padding: 10; text: ec2_status}";
                            break;
                        case 4:
                            component_qml += " Button{id: startstop; width: 80; height: 60; text: ec2_actions; font.pixelSize: 12; onClicked: tblInstances.startstopclick(" + row + ") }"+
                            " Button{anchors.left: startstop.right; width: 80; height: 60; text: 'Select'; font.pixelSize: 12; onClicked: tblInstances.instance_select_click(" + row + ") }"
                            ;
                    }

                    component_qml += "}}"

                    return Qt.createQmlObject(component_qml, parent)
                }
            }
        }

        function startstopclick(index) {
            var d2 = ec2CompDialog.createObject(win)

            d2.yes.connect(function () {
                instances_table_model.btnStartStopClick(index)
            })

            d2.visible = true
        }

        Component {
            id: ec2CompDialog

            MessageDialog {
                title: "Confirmation"
                text: "Are you sure?"
                onVisibleChanged: if (!visible)
                                      destroy(1)
                buttons: StandardButton.Yes | StandardButton.No
            }
        }

        function instance_select_click(index) {
            instances_table_model.btnSelectInstanceClick(index)
        }

        Rectangle {
            z: 3
            color: "#222222"
            y: tblInstances.contentY
            x: tblInstances.contentX
            width: tblInstances.leftMargin
            height: tblInstances.topMargin
        }

        Row {
            id: columnsHeader
            y: tblInstances.contentY
            z: 2
            Repeater {
                model: tblInstances.columns > 0 ? tblInstances.columns : 1
                Label {
                    width: tblInstances.columnWidthProvider(modelData)
                    height: 35
                    //height: 135 //Mobile
                    text: instances_table_model ? instances_table_model.headerData(modelData, Qt.Horizontal, Qt.DisplayRole) : ''
                    color: '#aaaaaa'
                    font.pixelSize: 13
                    //font.pixelSize: 63 //Mobile
                    padding: 10
                    verticalAlignment: Text.AlignVCenter

                    background: Rectangle {
                        color: "#333333"
                    }
                }
            }
        }
    }


    Button {
        id: btnRefreshInstances
        anchors.top: tblInstances.bottom
        anchors.topMargin: 5
        anchors.horizontalCenter: parent.horizontalCenter
        text: "Refresh EC2"

        onClicked: instances_table_model.btnRefreshInstancesClick()
    }
  } // Pane
} // Page
