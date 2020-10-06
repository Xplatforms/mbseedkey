import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import xplatforms.mbseedkey.exutils 1.0
import xplatforms.mbseedkey.ecuseedkeydll 1.0

Page
{
    title: qsTr("SeedKey Calculator / Generator")

    property ECUSeedKeyDLL ecu


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
                id: dlllabelid
                text: qsTr("Selected DLL:  ")
                font.weight: Font.DemiBold
            }

            Label
            {

                Layout.fillWidth: true
                text: ecu.DLLName
            }
        }

        RowLayout
        {
            Layout.fillWidth: true

            Label
            {
                Layout.minimumWidth: dlllabelid.width
                text: qsTr("ECU Name: ")
                font.weight: Font.DemiBold
            }

            Label
            {

                text: ecu.ECUName
            }
        }

        RowLayout
        {
            Layout.fillWidth: true

            Label
            {
                text: qsTr("Comment: ")
                font.weight: Font.DemiBold
            }
        }

        Text
        {
            Layout.minimumHeight: 60
            Layout.fillWidth: true
            maximumLineCount: 12
            text: ecu.Comment

        }




        Rectangle
        {
            Layout.fillHeight: true
        }
    }

}
