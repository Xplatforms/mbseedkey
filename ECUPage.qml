import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import xplatforms.mbseedkey.exutils 1.0
import xplatforms.mbseedkey.ecuseedkeydll 1.0

Page
{
    title: qsTr("SeedKey Calculator / Generator")

    property ECUSeedKeyDLL ecu

    function updateInputMasks()
    {        
        console.log("set mask: " + exutils.getInputMaskForSeedLen(use_custom_values_id.checked?custom_seed_len_id.value:ecu.seedLength(acc_combobox.currentValue)));
        seed_id.inputMask = exutils.getInputMaskForSeedLen(use_custom_values_id.checked?custom_seed_len_id.value:ecu.seedLength(acc_combobox.currentValue));
        key_id.inputMask = exutils.getInputMaskForSeedLen(use_custom_values_id.checked?custom_key_len_id.value:ecu.keyLength(acc_combobox.currentValue));
    }

    Timer
    {
        id: upd_timer_id
        repeat: false
        running: false
        interval: 200
        onTriggered: updateInputMasks()

    }

    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 15
        spacing: 12

        Component.onCompleted: updateInputMasks()

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

            Button
            {
                text: qsTr("go back")
                Layout.minimumWidth: 180
                font.weight: Font.DemiBold
                onClicked: stackView.pop();
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
            Layout.minimumHeight: 40
            Layout.fillWidth: true
            maximumLineCount: 6
            text: ecu.Comment

        }

        CheckBox
        {
            Layout.fillWidth: true
            id: use_custom_values_id
            checked: acc_combobox.count == 0
            text: qsTr("Set custom values for access level, seed and key length if dll doesn't provide them")
            font.italic: true          
            onCheckStateChanged: updateInputMasks();

        }

        RowLayout
        {
            Layout.fillWidth: true

            ColumnLayout
            {
                spacing: 12
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop

                RowLayout
                {
                    Layout.fillWidth: true
                    Label
                    {
                        id: acc_level_id
                        text: qsTr("Access Level: ")
                        //Layout.alignment: Qt.AlignBottom
                        font.weight: Font.DemiBold
                    }

                    ComboBox
                    {
                        id: acc_combobox
                        visible: !use_custom_values_id.checked
                        Layout.alignment: Qt.AlignLeft
                        focusPolicy: Qt.WheelFocus | Qt.ClickFocus
                        wheelEnabled: true
                        model: ecu.AccessTypesStringList
                        //validator: IntValidator {bottom: 0; top: 255;}
                        //model: ["Deutsch", "American English" ]
                        implicitWidth: spin_seed_id.width
                        //implicitHeight: spin_seed_id.height

                        //currentIndex: currentprofile.winsettings.lang=="de"?0:1

                        delegate: ItemDelegate
                        {
                            width: parent.width
                            contentItem: Label
                            {
                                text: modelData
                                elide: Text.ElideNone
                                verticalAlignment: Text.AlignVCenter
                                font.bold: acc_combobox.currentIndex === index
                            }
                            highlighted: acc_combobox.highlightedIndex === index
                        }

                        onActivated:
                        {

                        }

                        onDisplayTextChanged:
                        {
                            //console.log("combo changed" + acc_combobox.currentValue );
                            //updateInputMasks();
                            upd_timer_id.start();
                        }
                    }

                    TextField
                    {
                        id: custom_acc_lvl_id
                        visible: use_custom_values_id.checked
                        Layout.minimumWidth: 100
                        placeholderText: qsTr("HEX")
                        inputMethodHints: Qt.ImhUppercaseOnly
                        inputMask: ">HH;0"
                        selectByMouse: true
                        horizontalAlignment: TextInput.AlignHCenter

                        onFocusChanged: custom_acc_lvl_id.cursorPosition = 0
                    }
                }

                RowLayout
                {
                    Layout.fillWidth: true
                    Label
                    {
                        Layout.minimumWidth: acc_level_id.width
                        text: qsTr("Seed length:")
                        //Layout.alignment: Qt.AlignBottom
                        font.weight: Font.DemiBold
                    }

                    Label
                    {
                        id: spin_seed_id
                        visible: !use_custom_values_id.checked
                        Layout.alignment: Qt.AlignLeft
                        Layout.minimumWidth: 100
                        text: ecu.seedLength(acc_combobox.currentValue)
                    }

                    SpinBox
                    {
                        id: custom_seed_len_id
                        visible: use_custom_values_id.checked
                        Layout.minimumWidth: 100
                        from: 2
                        to: 128
                        value: 4
                        editable: true
                        wheelEnabled: true
                        onValueChanged: seed_id.inputMask = exutils.getInputMaskForSeedLen(value)
                    }
                }

                RowLayout
                {
                    Layout.fillWidth: true
                    Label
                    {
                        Layout.minimumWidth: acc_level_id.width
                        text: qsTr("Key length:")
                        //Layout.alignment: Qt.AlignBottom
                        font.weight: Font.DemiBold
                    }

                    Label
                    {
                        visible: !use_custom_values_id.checked
                        Layout.alignment: Qt.AlignLeft
                        Layout.minimumWidth: 100
                        text: ecu.keyLength(acc_combobox.currentValue)
                    }

                    SpinBox
                    {
                        id: custom_key_len_id
                        visible: use_custom_values_id.checked
                        Layout.minimumWidth: 100
                        from: 2
                        to: 128
                        value: 4
                        editable: true
                        wheelEnabled: true
                        onValueChanged: key_id.inputMask = exutils.getInputMaskForSeedLen(value)
                    }
                }
            }

            Rectangle
            {
                Layout.fillHeight: true
                Layout.leftMargin: 10
                Layout.rightMargin: 10
                width: 2
                radius: 4
                color: "lightgrey"
            }

            ColumnLayout
            {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignTop


                Label
                {
                    text: qsTr("SEED: ")
                }

                TextField
                {
                    id: seed_id
                    Layout.fillWidth: true
                    selectByMouse: true
                    inputMask: exutils.getInputMaskForSeedLen(use_custom_values_id.checked?custom_seed_len_id.value:ecu.seedLength(acc_combobox.currentValue))
                    //validator: ExInputValidator{}
                    onFocusChanged:
                    {
                        if(focus)cursorPosition = 0;
                    }
                    onEditingFinished:
                    {
                        //text = exutils.patchSeedString(text)
                    }
                }


                Label
                {
                    text: qsTr("KEY: ")
                }

                TextField
                {
                    id: key_id
                    Layout.fillWidth: true
                    readOnly: true
                    selectByMouse: true
                    inputMask: exutils.getInputMaskForSeedLen(use_custom_values_id.checked?custom_key_len_id.value:ecu.keyLength(acc_combobox.currentValue))
                    onFocusChanged: focus?selectAll():deselect()


                }


                Button
                {
                    text: qsTr("Generate key")
                    font.weight: Font.DemiBold
                    Layout.minimumWidth: 180

                    onClicked: key_id.text = ecu.generateKeyFromSeed(seed_id.displayText,
                                                       use_custom_values_id.checked?custom_acc_lvl_id.text:acc_combobox.currentValue,
                                                       use_custom_values_id.checked?custom_key_len_id.value.toString():ecu.keyLength(acc_combobox.currentValue) )
                }


            }
        }


    }

}
