import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import xplatforms.mbseedkey.exutils 1.0
import xplatforms.mbseedkey.ecuseedkeydll 1.0

Page
{
    title: qsTr("Select Window")


    function searchFor(text)
    {
        exutils.searchFor(text);
    }

    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 12

        RowLayout
        {
            Layout.fillWidth: true

            Label
            {
                id: labeldllfolder
                text: qsTr("SeedKey DLL's folder: ")
            }

            Rectangle
            {
                Layout.fillWidth: true
                Layout.preferredWidth: dllloadfoldertext.implicitWidth
                Layout.preferredHeight: 25
                Layout.minimumWidth: 25
                Layout.maximumWidth: dllloadfoldertext.implicitWidth

                Label
                {
                    id: dllloadfoldertext
                    anchors.fill: parent
                    font.weight: Font.DemiBold
                    elide: Text.ElideMiddle
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: exutils.foldername
                }
            }
        }

        Button
        {
            text: qsTr("Change DLL folder")
            onClicked:
            {
                dllselect.open();
            }

            Connections
            {
                target: dllselect
                function onAccepted()
                {
                    var ordner = dllselect.folder.toString();
                    ordner = ordner.replace("file:///", "")
                    console.log("User has selected " + ordner);
                    exutils.foldername = ordner
                }
            }
        }

        RowLayout
        {
            Layout.topMargin: 15
            Layout.fillWidth: true
            TextField
            {
                id: searchfield
                Layout.fillWidth: true
                placeholderText: qsTr("Search in DLL's Name")
                selectByMouse: true
                onTextChanged: {
                    searchFor(text);
                }
            }

            Button
            {
                text: qsTr("OK")
                onClicked: {searchFor(searchfield.text)}
            }
        }

        RowLayout
        {

            Rectangle
            {
                Layout.fillHeight: true
                Layout.fillWidth: true

                //color: "red"

                border.width: 1
                border.color: "lightgrey"

                //color: "red"

                ListView
                {
                    id: listView
                    anchors.fill: parent
                    anchors.margins: 5
                    model: exutils.dlls

                    focus: true
                    highlight: Rectangle { color: "#cfdbeb"; radius: 2 }
                    highlightFollowsCurrentItem: true
                    clip: true

                    delegate: ItemDelegate
                    {
                        text: modelData
                        width: listView.width

                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked:
                            {
                                console.log("Current listview index:" + index)
                                listView.currentIndex = index;
                                exutils.selectDll(index)
                            }
                        }
                    }
                }

                Component.onCompleted:
                {
                    exutils.selectDll(listView.currentIndex);
                }
            }

            Rectangle
            {
                Layout.fillHeight: true
                Layout.fillWidth: true
                //color: "green"
                //visible: exutils.seedkeydll != null

                ColumnLayout
                {
                    anchors.fill: parent


                    Label
                    {
                        text: qsTr("ECU Name:")
                        font.weight: Font.DemiBold
                    }
                    Label
                    {
                        text: exutils.seedkeydll?exutils.seedkeydll.ECUName:qsTr("not loaded")
                    }

                    Label
                    {
                        text: qsTr("Access Levels:")
                        font.weight: Font.DemiBold
                    }
                    Label
                    {
                        text: exutils.seedkeydll?exutils.seedkeydll.AccessTypesString:qsTr("not loaded")
                    }

                    Label
                    {
                        text: qsTr("Comment:")
                        font.weight: Font.DemiBold
                    }
                    Text
                    {
                        Layout.fillHeight: true
                        text: exutils.seedkeydll?exutils.seedkeydll.Comment:qsTr("not loaded")
                    }

                    Rectangle
                    {
                        height: 5
                    }


                    Button
                    {
                        Layout.alignment: Qt.AlignBottom
                        text: qsTr(" Select this DLL ")

                        onClicked:
                        {
                            // stackView.push("ProfilePage.qml", {"currentprofile": appsettings.getProfileByID(appsettings.defaultProfileID())})
                            stackView.push("ECUPage.qml", {"ecu": exutils.seedkeydll})
                        }
                    }
                }
            }

        }

        Rectangle
        {
            height: 1
        }

    }
}
