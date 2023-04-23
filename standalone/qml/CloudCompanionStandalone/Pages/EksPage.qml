import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import Qt.labs.platform
import Qaterial as Qaterial

Qaterial.Page {
    id: _root

    property bool enabled: true

    Qaterial.Pane {
        anchors.centerIn: parent
        elevation: 20
        height: parent.height - 40
        implicitHeight: height
        padding: 0
        width: parent.width - 5

        Column {
            id: _column

            anchors.horizontalCenter: parent.horizontalCenter
            width: 1000

            Settings {
                property alias csvColumn: csv_column_no.text
                property alias csvDelimiter: csv_delimiter_combo.currentIndex
                property alias jsonField: json_field.text
                property alias kubernetesBasePath: _kubernetesBasePath.text
                property alias logParser: log_parser_combo.currentIndex

                category: "settings/kubernetes"
            }
            Row {
                TableView {
                    id: podsTable

                    function delete_click(index) {
                        eks_pods_table_model.btnDelete(index);
                    }
                    function instance_select_click(index) {
                        eks_pods_table_model.btnSelectInstanceClick(index);
                    }
                    function log_click(index) {
                        eks_pods_table_model.btnLog(index);
                    }

                    clip: true
                    columnWidthProvider: function (column) {
                        if (column === 0) {
                            return 240;
                        } else if (column === 1) {
                            return 120;
                        } else if (column === 2) {
                            return 120;
                        } else if (column === 3) {
                            return 240;
                        } else if (column === 4) {
                            return 120;
                        } else if (column === 5) {
                            return 200;
                        }
                    }
                    height: 200
                    model: eks_pods_table_model
                    rowHeightProvider: function () {
                        return 40;
                    }
                    topMargin: columnsHeader.implicitHeight
                    width: 1000

                    delegate: Component {
                        Loader {
                            sourceComponent: {
                                if (model === undefined || model === null || parent === null) {
                                    return;
                                }
                                var component_qml = "import QtQuick; import QtQuick.Controls; Component { Rectangle { border.color: 'black';";
                                switch (column) {
                                case 0:
                                    component_qml += " Text{padding: 10; text: eks_name}";
                                    break;
                                case 1:
                                    component_qml += " Text{padding: 10; text: eks_namespace}";
                                    break;
                                case 2:
                                    component_qml += " Text{padding: 10; text: eks_pod_ip}";
                                    break;
                                case 3:
                                    component_qml += " Text{padding: 10; text: eks_created_at}";
                                    break;
                                case 4:
                                    component_qml += " Text{padding: 10; text: eks_status}";
                                    break;
                                case 5:
                                    component_qml += " Button{id: delete_pod; width: 75; height: 40; text: 'Delete'; font.pixelSize: 12; onClicked: {podsTable.delete_click(" + row + ")} }";
                                    component_qml += " Button{id: log_pod; width: 75; height: 40; text: 'Log'; font.pixelSize: 12; anchors.left: delete_pod.right ; onClicked: {podsTable.log_click(" + row + ")} }";
                                }
                                component_qml += "}}";
                                return Qt.createQmlObject(component_qml, parent);
                            }
                        }
                    }

                    Rectangle {
                        color: "#222222"
                        height: podsTable.topMargin
                        width: podsTable.leftMargin
                        x: podsTable.contentX
                        y: podsTable.contentY
                        z: 3
                    }

                    Row {
                        id: columnsHeader

                        y: podsTable.contentY
                        z: 2

                        Repeater {
                            model: podsTable.columns > 0 ? podsTable.columns : 1

                            Label {
                                color: '#aaaaaa'
                                font.pixelSize: 13
                                height: 35
                                //font.pixelSize: 63 //Mobile
                                padding: 10
                                //height: 135 //Mobile
                                text: eks_pods_table_model ? eks_pods_table_model.headerData(modelData, Qt.Horizontal, Qt.DisplayRole) : ''
                                verticalAlignment: Text.AlignVCenter
                                width: podsTable.columnWidthProvider(modelData)

                                background: Rectangle {
                                    color: "#333333"
                                }
                            }
                        }
                    }

                    Component {
                        id: eksCompDialog

                        MessageDialog {
                            buttons: StandardButton.Yes | StandardButton.No
                            text: "Are you sure?"
                            title: "Confirmation"

                            onVisibleChanged: if (!visible) {
                                destroy();
                            }
                        }
                    }
                }
            }
            Row {
                Qaterial.TextField {
                    id: _kubernetesBasePath

                    title: "Kubernetes Base Path"
                    width: _column.width * 0.8
                }
            }
            Row {
                Button {
                    id: btnRefreshInstances

                    text: "Refresh Pods"

                    onClicked: eks_pods_table_model.btnRefreshPodsClick()
                }
                Button {
                    id: btnWatchInstances

                    property bool watching: false

                    text: "Watch Pods"

                    onClicked: {
                        if (watching) {
                            this.watching = false;
                            this.text = "Watch Pods";
                        } else {
                            this.watching = true;
                            this.text = "Stop Watch";
                        }
                        eks_pods_table_model.btnWatchPodsClick(this.watching);
                    }
                }
                Qaterial.Label {
                    id: log_parser_label

                    height: parent.height
                    text: "Log Format: "
                    verticalAlignment: Qt.AlignVCenter
                }
                Qaterial.ComboBox {
                    id: log_parser_combo

                    model: ["Auto", "JSON", "CSV", "Text"]
                }
                Qaterial.Label {
                    id: csv_delimiter_label

                    height: parent.height
                    text: "CSV Delimiter: "
                    verticalAlignment: Qt.AlignVCenter
                }
                Qaterial.ComboBox {
                    id: csv_delimiter_combo

                    model: ["COMMA", "TAB", "SPACE"]
                }
                Qaterial.TextField {
                    id: csv_column_no

                    text: "1"
                    title: "CSV Column No:"
                }
                Qaterial.TextField {
                    id: json_field

                    text: "msg"
                    title: "JSON Field:"
                }
            }
        }
    }
}
