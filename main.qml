import QtQuick 2.15
import QtQuick.Controls 2.15

ApplicationWindow
{
    id: window

    width: 780
    height: 420
    visible: true
    title: qsTr("Stack")


    StackView
    {
        id: stackView
        initialItem: "MainPage.qml"
        anchors.fill: parent
    }
}
