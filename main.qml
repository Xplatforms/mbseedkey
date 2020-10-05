import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3


ApplicationWindow
{
    id: window

    width: 780
    height: 480
    visible: true
    title: qsTr(" Xplatforms / mbseedkey ")

    FileDialog
    {
        id: dllselect
        title: qsTr("Please select folder with SeedKey Dll's")
        selectFolder: true

        onAccepted:
        {
            console.log(folder);
        }

    }

    StackView
    {
        id: stackView
        initialItem: "MainPage.qml"
        anchors.fill: parent
    }
}
