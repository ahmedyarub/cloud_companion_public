import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import Qt5Compat.GraphicalEffects
import Qaterial as Qaterial
import CloudCompanionStandalone as CloudCompanionStandalone
import CloudCompanionStandalone.Pages

Qaterial.ApplicationWindow {
    id: win

    height: 680 //Desktop
    title: "Cloud Companion"
    visible: true
    //    visibility: Window.FullScreen //Mobile
    width: 1010 //Desktop

    footer: Qaterial.ScrollableTabBar {
        id: _tabBar

        currentIndex: _swipeView.currentIndex
        elevation: 20

        model: ListModel {
            ListElement {
                text: "Credentials"
            }
            ListElement {
                text: "Docker"
            }
            ListElement {
                text: "ECR"
            }
            ListElement {
                text: "EC2"
            }
            ListElement {
                text: "Kubernetes"
            }
            ListElement {
                text: "Deployment"
            }
        } // ListModel
    } // FixedTabBar
    menuBar: Qaterial.MenuBar {
        visible: true

        Qaterial.Menu {
            title: qsTr("View")

            Qaterial.MenuItem {
                checkable: true
                checked: true
                text: qsTr("Dark theme")

                onTriggered: {
                    Qaterial.Style.theme = checked ? Qaterial.Style.Theme.Dark : Qaterial.Style.Theme.Light;
                }
            } // MenuItem
        } // Menu
        Qaterial.Menu {
            title: qsTr("Help")

            Qaterial.MenuItem {
                text: qsTr("About")

                onTriggered: Qaterial.DialogManager.showDialog({
                        "text": "Version: 1.0.0 (alpha1)",
                        "title": qsTr("About Cloud Companion"),
                        "iconSource": "qrc:/CloudCompanionStandalone/icon.svg",
                        "standardButtons": Dialog.Ok
                    }) // DialogManager.showDialog
            } // MenuItem
        } // Menu
    } // MenuBar

    Qaterial.SwipeView {
        id: _swipeView

        anchors.fill: parent
        currentIndex: _tabBar.currentIndex

        CredentialsPage {
            height: _swipeView.height
            width: _swipeView.width
        } // FixedTabBarPage
        DockerPage {
            id: dockerPage

            height: _swipeView.height
            width: _swipeView.width
        } // FixedTabBarPage
        EcrPage {
            height: _swipeView.height
            width: _swipeView.width
        } // FooterTabBarPage
        Ec2Page {
            height: _swipeView.height
            width: _swipeView.width
        } // FixedTabBarPage
        EksPage {
            height: _swipeView.height
            width: _swipeView.width
        } // FixedTabBarPage
        DeployPage {
            height: _swipeView.height
            width: _swipeView.width
        } // ScrollableTabBarPage
    } // SwipeView
    StatusView {
        id: _statusView

        anchors.bottom: parent.bottom
        errorString: ""
        file: ""
        height: 250
        width: parent.width

        Connections {
            function onClearLog() {
                _statusView.clear();
            }
            function onNewLog(msg) {
                const color = function () {
                    if (msg.includes("debug"))
                        return Qaterial.Style.blue;
                    if (msg.includes("info"))
                        return Qaterial.Style.green;
                    if (msg.includes("warning"))
                        return Qaterial.Style.orange;
                    if (msg.includes("error"))
                        return Qaterial.Style.red;
                    return Qaterial.Style.primaryTextColor();
                }();
                _statusView.append(`<font style='color:${color};'>${msg}</font>`);
            }

            target: CloudCompanionStandalone.Logger
        }
    } // StatusView
    Connections {
        function onOpenTroubleshooter(problem) {
            solutionsList.clear();
            solutionsList.problem = problem;
            for (var solution of solutions_repo.getSolutions(problem)) {
                solutionsList.append({
                        "text": solution.friendly_name,
                        "secondaryText": solution.parameter
                    });
            }
            Qaterial.DialogManager.openFromComponent(solutionsDialogComponent);
        }

        target: solutions_repo
    }
    Component {
        id: solutionsDialogComponent

        Qaterial.RadioDialog {
            currentIndex: solutionsDialogComponent.modelIndex ? solutionsDialogComponent.modelIndex : 0
            model: solutionsList
            title: "Troubleshooter"

            Component.onCompleted: {
                open();
            }
            onAccepted: {
                solutions_repo.runAction(solutionsList.problem, solutionsList.get(currentIndex).text);
            }
        } // RadioDialog
    } // Component
    ListModel {
        id: solutionsList

        property string problem: ''
    }
    Connections {
        function onTroubleshootPushImage() {
            solutionsList.clear();
            for (var solution of solutions_repo.getSolutions('docker_not_found')) {
                solutionsList.append({
                        "text": solution.friendly_name,
                        "secondaryText": solution.parameter
                    });
            }
            Qaterial.DialogManager.openFromComponent(solutionsDialogComponent);
        }

        target: docker_client
    }
    Component {
        id: troubleshootPushDocker

        Qaterial.RadioDialog {
            title: "Docker Image Push Troubleshooter"

            model: ListModel {
                ListElement {
                    secondaryText: "List the ECR repositories associated with this account and choose one of them"
                    text: "Choose another repo"
                }
            } // ListMOdel

            Component.onCompleted: open()
            onAccepted: {
                console.log("Accept " + currentIndex);
                ecr_list_model.imagePushTroubleshootAction(currentIndex);
            }
            onRejected: console.log("Cancel")
        } // RadioDialog
    } // Component

    /* Stepper
        Qaterial.Stepper
        {
            id: stepper

            x: root.verticalStepper ? parent.width/2 - _row.width/2 : 0
            y: root.verticalStepper ? 0 : parent.height/2 - 50
            width: parent.width
            height: parent.height

            readonly property int indicatorSize: 48

            // Dimension properties
            indicatorWidth: indicatorSize
            indicatorHeight: indicatorSize

            contentItemWidth: 100
            contentItemHeight: 20

            // General properties
            clickable: true
            vertical: root.verticalStepper

            model: Qaterial.StepperModel
            {
              Qaterial.StepperElement
              {
                text: "Create Artifact";
                supportingText: "Create a Docker image for your project and push it to registry"
                property string iconName: Qaterial.Icons.docker
              }
              Qaterial.StepperElement
              {
                text: "Select Artifact";
                supportingText: "Select a Docker image to deploy from ECR"
                optional: true
                property string iconName: Qaterial.Icons.cube
              }
              Qaterial.StepperElement
              {
                text: "Prepare Your Instance";
                supportingText: "Create or start an EC2 instance and select it for deployemnt"
                property string iconName: Qaterial.Icons.server
              }
              Qaterial.StepperElement
              {
                text: "Deploy";
                supportingText: "Start the actual deployment process"
                property string iconName: Qaterial.Icons.upload
              }
            } // StepperModel

            nextRole: "newNextElementRole"

            separator: Rectangle
            {
              property Qaterial.StepperElement previous
              property Qaterial.StepperElement newNextElementRole
              property int index
              property bool highlighted:
              {
                // If previous null just check if next is done
                if(previous == null)
                  return newNextElementRole.done
                // Idem for next
                if(newNextElementRole == null)
                  return previous.done
                // If both aren't null
                return previous.done && newNextElementRole.done
              }
              color: highlighted ? Qaterial.Style.accentColor : Qaterial.Style.dividersColor()
              height: 8; radius: 4
            } // Rectangle

            indicator: Qaterial.ColorIcon
            {
              anchors.centerIn: parent
              iconSize: stepper.indicatorSize

              // Properties injected by the HorizontalStepper when detected allowing to acces to data as
              // element: the element displayed
              // index: the index of the element displayed
              // done: the "done" property of the model
              // alertMessage: the "alertMessage" property of the model
              property Qaterial.StepperElement element
              property bool done: element ? element.done : false
              property string alertMessage: element ? element.alertMessage : ""

              readonly property bool isAlertStep: alertMessage !== ""

              color:
              {
                if(isAlertStep)
                  return Qaterial.Style.red
                return done ? Qaterial.Style.accentColor : Qaterial.Style.dividersColor()
              }
              source: element.iconName
            } // ColorIcon

            contentItem: Qaterial.Label
            {
              width: 100
              height: 20

              property Qaterial.StepperElement element
              property int index
              property bool done: element ? element.done : false
              property string alertMessage: element ? element.alertMessage : ""

              readonly property bool isCurrent: index === stepper.currentIndex
              readonly property bool isAlertStep: alertMessage !== ""

              text:
              {
                if(isAlertStep)
                  return done ? element.text : alertMessage
                return element.text
              }
              font.bold: isCurrent
              horizontalAlignment: stepper.vertical ? Text.AlignLeft : Text.AlignHCenter
              color:
              {
                if(isAlertStep)
                  return Qaterial.Style.red
                return isCurrent ? Qaterial.Style.accentColor : Qaterial.Style.primaryTextColor()
              }
            } // Label
        } // HorizontalStepper

        footer: Item
        {
        height: supportingTextLabel.height + _row.height + 20
        Qaterial.Label
        {
          id: supportingTextLabel
          width: parent.width

          text: stepper.currentElement.supportingText
          textType: Qaterial.Style.TextType.Body2
          horizontalAlignment: Text.AlignHCenter

          wrapMode: Text.Wrap
          elide: Text.ElideRight
          maximumLineCount: stepper.vertical ? 10 : 3
        } // Label

        Row
        {
          id: _row
          y: supportingTextLabel.height + 10
          anchors.horizontalCenter: parent.horizontalCenter
          spacing: 10

          Qaterial.OutlineButton
          {
            id: _backButton
            text: "Back"
            onClicked: stepper.currentIndex >= 1 ? stepper.currentIndex -- : console.log(`currentIndex min`)
          } // OutlineButton
          Qaterial.RaisedButton
          {
            text: "Next"
            onClicked: stepper.currentIndex < stepper.count-1 ? stepper.currentIndex ++ : console.log(`currentIndex max`)
          } // RaisedButton
          Qaterial.OutlineButton
          {
            visible: stepper.currentElement.optional
            text: "Skip"
            onClicked: stepper.currentIndex < stepper.count-1 ? stepper.currentIndex ++ : console.log(`currentIndex max`)
          } // OutlineButton

          Qaterial.RaisedButton
          {
            property bool done: stepper.currentElement.done

            text:
            {
              if(stepper.currentIndex === stepper.count-1)
                return done ? "Cancel" : "Finish"
              return done ? "Cancel" : "Done"
            }
            onClicked:
            {
              stepper.currentElement.done = !stepper.currentElement.done

              // If this is the last Steps you want to know it
              if(stepper.currentIndex === stepper.count-1 && done)
              {
                let finished = true
                let unfinishedStep = -1

                /// Check if every step are done if they aren't optional
                for (let i = 0; i <= stepper.count-1; i++)
                {
                  if(!stepper.model.get(i).done && !stepper.model.get(i).optional)
                  {
                    finished = false; unfinishedStep = i+1
                  }
                }
                if(finished)
                  console.log(`Finish !`)
                else
                {
                  console.log(`You haven't finished the step ${unfinishedStep}! You have to do it to finish the process`)
                  stepper.currentElement.done = !stepper.currentElement.done
                }
              }
              // When Done is clicked, go to next step
              if(stepper.currentIndex < stepper.count-1 && done)
                stepper.currentIndex ++
            }
          } // RaisedButton
        } // Row
        } // footer : Item
        */
}
