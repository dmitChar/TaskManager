import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Dialog
{
    id: dialog
    title: "Add task"
    modal: true
    standardButtons: "NoButton"
    width: 400

    ColumnLayout
    {
        anchors.fill: parent
        spacing: 10

        TextField
        {
            id: titleField
            placeholderText: "Task title"
            Layout.fillWidth: true
        }

        ComboBox
        {
            id: priorityBox
            Layout.fillWidth: true
            model: [
                { text: "High priority", value: 1 },
                { text: "Medium priority", value: 2 },
                { text: "Low priority", value: 3 }
            ]
            textRole: "text"
            valueRole: "value"
            currentIndex: 1
        }

        //Теги
        ColumnLayout
        {
            Flow
            {
                spacing: 6
                Repeater
                {
                    model: tagsModel
                    delegate: Rectangle
                    {
                        height: 22
                        width: tagText.width + 20
                        color: selected ? "#2196F3" : "#e0e0e0"
                        radius: 11


                        Text
                        {
                            id: tagText
                            anchors.centerIn: parent
                            text: name
                            color: selected ? "white" : "black"
                            font.pixelSize: 13
                        }
                        MouseArea
                        {
                            anchors.fill: parent
                            onClicked:
                            {
                                tagsModel.setProperty(index, "selected", !selected)
                            }
                        }
                    }
                }
            }
        }

        //Описание
        TextArea
        {
            id: descriptionField
            placeholderText: "Description"
            Layout.fillWidth: true
            Layout.preferredHeight: 80
            wrapMode: TextArea.Wrap
        }
        RowLayout
        {
            spacing: 12
            Layout.fillWidth: true
            Button
            {
                id: cancelBtn
                text: "Отмена"
                onClicked: dialog.close()
            }
            Button
            {
                id: acceptBtn
                text: "Добавить"
                enabled: titleField.text.length > 0
            }
        }
    }


    //---------------------------

    //Модель тегов
    ListModel
    {
        id: tagsModel
        ListElement{name: "work"; selected: false}
        ListElement{name: "study"; selected: false}
        ListElement{name: "urgent"; selected: false}
    }
    //Функция возврата выбранных тегов
    function selectedTags()
    {
        let result = []
        for (let i = 0; i < tagsModel.count; ++i)
        {
            if (tagsModel.get(i).selected)
                result.push(tagsModel.get(i).name)
        }
        return result
    }
}

