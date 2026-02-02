import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window
{
    id: editWindow
    minimumWidth: 520
    minimumHeight: 540
    title: "Edit Task"
    modality: Qt.ApplicationModal
    visible: false
    color: "#eef1f5"
    flags: Qt.Dialog | Qt.WindowCloseButtonHint | Qt.WindowTitleHint

    x: Screen.width / 2 - width / 2
    y: Screen.height / 2 - height / 2

    property int taskId
    property string status

    function openEditWindow(id, sourceStatus)
    {
        taskId = id
        console.log("Open edit window taskId = ", id)
        editWindow.status = sourceStatus
        let taskData = taskController.getTask(id)
        if (taskData.id !== undefined)
        {
            console.log("Task title:", taskData.title)
            console.log("Status:", taskData.status)
            console.log("Tags:", taskData.tags)
            titleField.text = taskData.title
            descriptionArea.text = taskData.description
            priorityBox.currentIndex = taskData.priority - 1
            setTags(taskData.tags)
        }
        show()
        raise()
        titleField.forceActiveFocus()
    }

    // Обработка нажатия Escape для закрытия
    Shortcut {
        sequence: StandardKey.Cancel
        onActivated: editWindow.close()
    }

    // Обработка Enter/Return для сохранения
    Shortcut {
        sequence: "Ctrl+Return"
        onActivated: saveTask()
    }

    Rectangle
    {
        anchors.centerIn: parent
        width: parent.width - 40
        height: parent.height - 40
        radius: 16
        color: "#ffffff"

        ColumnLayout
        {
            anchors.fill: parent
            anchors.margins: 24
            spacing: 16

            // Заголовок окна
            RowLayout {
                Layout.fillWidth: true
                spacing: 12

                Rectangle {
                    width: 40
                    height: 40
                    radius: 20
                    color: "#e3f2fd"

                    Text {
                        anchors.centerIn: parent
                        text: "✏️"
                        font.pixelSize: 20
                    }
                }

                Label {
                    text: "Edit Task"
                    font.pixelSize: 24
                    font.bold: true
                    color: "#2c3e50"
                    Layout.fillWidth: true
                }

                // Кнопка закрытия
                Button {
                    implicitWidth: 32
                    implicitHeight: 32

                    contentItem: Text {
                        text: "✕"
                        font.pixelSize: 16
                        color: "black"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        radius: 16
                        color: parent.hovered ? "#C0C0C0" : "transparent"

                        Behavior on color {
                            ColorAnimation { duration: 150 }
                        }
                    }

                    onClicked: editWindow.close()
                }
            }

            // Название с лейблом
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 6

                Label {
                    text: "Title *"
                    font.pixelSize: 13
                    font.bold: true
                    color: "#34495e"
                }

                TextField {
                    id: titleField
                    Layout.fillWidth: true
                    placeholderText: "Enter task title..."
                    font.pixelSize: 14
                    color: "black"
                    selectByMouse: true

                    leftPadding: 12
                    rightPadding: 12
                    topPadding: 10
                    bottomPadding: 10

                    background: Rectangle
                    {
                        radius: 8
                        color: titleField.focus ? "#ffffff" : "#f8f9fa"
                        border.color: {
                            if (!titleField.text && titleField.focus) return "#e74c3c"
                            if (titleField.focus) return "#3498db"
                            return "#dfe4ea"
                        }
                        border.width: titleField.focus ? 2 : 1

                        Behavior on border.color {
                            ColorAnimation { duration: 150 }
                        }

                        Behavior on color {
                            ColorAnimation { duration: 150 }
                        }
                    }
                }

                // Сообщение об ошибке
                Label {
                    text: "Title is required"
                    font.pixelSize: 11
                    color: "#e74c3c"
                    visible: !titleField.text && titleField.focus
                    Layout.topMargin: -4
                }
            }

            // Приоритет с лейблом
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 6

                Label {
                    text: "Priority"
                    font.pixelSize: 13
                    font.bold: true
                    color: "#34495e"
                }

                ComboBox {
                    id: priorityBox
                    Layout.fillWidth: true
                    model: [
                        { text: "🔴 High Priority", value: 1, color: "#e74c3c" },
                        { text: "🟠 Medium Priority", value: 2, color: "#f39c12" },
                        { text: "🟢 Low Priority", value: 3, color: "#2ecc71" }
                    ]
                    textRole: "text"
                    valueRole: "value"
                    currentIndex: 1

                    leftPadding: 12
                    rightPadding: 12
                    topPadding: 10
                    bottomPadding: 10

                    contentItem: Text
                    {
                        text: priorityBox.displayText
                        font: priorityBox.font
                        color: "#2c3e50"
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }

                    background: Rectangle {
                        radius: 8
                        color: priorityBox.pressed ? "#ecf0f1" : "#f8f9fa"
                        border.color: priorityBox.activeFocus ? "#3498db" : "#dfe4ea"
                        border.width: priorityBox.activeFocus ? 2 : 1

                        Behavior on border.color {
                            ColorAnimation { duration: 200 }
                        }
                    }

                    delegate: ItemDelegate {
                        width: priorityBox.width

                        contentItem: Text
                        {
                            text: modelData.text
                            color: "#2c3e50"
                            font: priorityBox.font
                            elide: Text.ElideRight
                            verticalAlignment: Text.AlignVCenter
                            Layout.fillWidth: true
                        }

                        background: Rectangle
                        {
                            color: highlighted ? "#ecf0f1" : "transparent"

                            Behavior on color {
                                ColorAnimation { duration: 200 }
                            }
                        }

                        highlighted: priorityBox.highlightedIndex === index
                    }

                    popup: Popup {
                        y: priorityBox.height + 4
                        width: priorityBox.width
                        implicitHeight: contentItem.implicitHeight + 16
                        padding: 8

                        contentItem: ListView {
                            clip: true
                            implicitHeight: contentHeight
                            model: priorityBox.popup.visible ? priorityBox.delegateModel : null
                            currentIndex: priorityBox.highlightedIndex

                            ScrollIndicator.vertical: ScrollIndicator { }
                        }

                        background: Rectangle {
                            color: "white"
                            border.color: "#dfe4ea"
                            border.width: 1
                            radius: 8
                        }
                    }
                }
            }

            // Теги
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 8

                Label {
                    text: "Tags"
                    font.pixelSize: 13
                    font.bold: true
                    color: "#34495e"
                }

                Flow {
                    Layout.fillWidth: true
                    spacing: 8

                    Repeater {
                        model: tagsModel
                        delegate: Rectangle {
                            height: 32
                            radius: 16
                            width: tagText.implicitWidth + 24
                            color: selected ? getTagColor(name) : "#f8f9fa"
                            border.width: 1
                            border.color: selected ? Qt.darker(getTagColor(name), 1.1) : "#dfe4ea"

                            property bool hovered: false
                            scale: hovered ? 1.05 : 1.0

                            Behavior on color {
                                ColorAnimation { duration: 150 }
                            }

                            Behavior on scale {
                                NumberAnimation { duration: 100 }
                            }

                            Text {
                                id: tagText
                                anchors.centerIn: parent
                                text: name
                                color: selected ? "white" : "#7f8c8d"
                                font.pixelSize: 13
                                font.bold: selected
                            }

                            MouseArea {
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor

                                onEntered: parent.hovered = true
                                onExited: parent.hovered = false

                                onClicked: {
                                    tagsModel.setProperty(index, "selected", !selected)
                                }
                            }
                        }
                    }
                }
            }

            // Описание
            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: 6

                Label {
                    text: "Description"
                    font.pixelSize: 13
                    font.bold: true
                    color: "#34495e"
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.minimumHeight: 100
                    clip: true

                    TextArea {
                        id: descriptionArea
                        placeholderText: "Add a detailed description..."
                        font.pixelSize: 14
                        wrapMode: TextArea.Wrap
                        selectByMouse: true
                        color: "black"

                        leftPadding: 12
                        rightPadding: 12
                        topPadding: 10
                        bottomPadding: 10

                        background: Rectangle {
                            radius: 8
                            color: descriptionArea.focus ? "#ffffff" : "#f8f9fa"
                            border.color: descriptionArea.focus ? "#3498db" : "#dfe4ea"
                            border.width: descriptionArea.focus ? 2 : 1

                            Behavior on border.color {
                                ColorAnimation { duration: 150 }
                            }

                            Behavior on color {
                                ColorAnimation { duration: 150 }
                            }
                        }
                    }
                }
            }

            // Кнопки
            RowLayout {
                Layout.fillWidth: true
                Layout.topMargin: 8
                spacing: 12

                Item { Layout.fillWidth: true }

                Button {
                    text: "Cancel"
                    implicitWidth: 100
                    implicitHeight: 40

                    contentItem: Text {
                        text: parent.text
                        font.pixelSize: 14
                        font.bold: true
                        color: "#7f8c8d"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        radius: 8
                        color: parent.hovered ? "#ecf0f1" : "transparent"
                        border.color: "#dfe4ea"
                        border.width: 2

                        Behavior on color {
                            ColorAnimation { duration: 150 }
                        }
                    }

                    onClicked: editWindow.close()
                }

                Button {
                    text: "Save Changes"
                    implicitWidth: 140
                    implicitHeight: 40
                    enabled: titleField.text.trim() !== "" // Валидация

                    contentItem: Text {
                        text: parent.text
                        font.pixelSize: 14
                        font.bold: true
                        color: parent.enabled ? "white" : "#95a5a6"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle {
                        radius: 8
                        color: {
                            if (!parent.enabled) return "#bdc3c7"
                            if (parent.pressed) return "#2980b9"
                            if (parent.hovered) return "#5dade2"
                            return "#3498db"
                        }

                        Behavior on color {
                            ColorAnimation { duration: 150 }
                        }
                    }

                    onClicked: saveTask()
                }
            }
        }
    }

    // Модель тегов
    ListModel {
        id: tagsModel
        ListElement { name: "work"; selected: false }
        ListElement { name: "study"; selected: false }
        ListElement { name: "urgent"; selected: false }
    }

    // Функции
    function selectedTags() {
        let result = []
        for (let i = 0; i < tagsModel.count; i++) {
            if (tagsModel.get(i).selected)
                result.push(tagsModel.get(i).name)
        }
        return result
    }

    function setTags(selectedTagNames) {
        for (let i = 0; i < tagsModel.count; ++i) {
            tagsModel.setProperty(i, "selected", false)
        }
        for (let tag of selectedTagNames) {
            for (let i = 0; i < tagsModel.count; ++i) {
                if (tag === tagsModel.get(i).name)
                    tagsModel.setProperty(i, "selected", true)
            }
        }
    }

    function getTagColor(tagName) {
        if (tagName === "work") return "#3498db"
        if (tagName === "study") return "#2ecc71"
        if (tagName === "urgent") return "#e74c3c"
        return "#95a5a6"
    }

    function saveTask() {
        if (titleField.text.trim() === "") {
            titleField.forceActiveFocus()
            return
        }

        taskController.updateTask(
            taskId,
            status,
            titleField.text,
            descriptionArea.text,
            selectedTags(),
            //priorityBox.model[priorityBox.currentIndex].value
            priorityBox.currentValue
        )
        editWindow.close()
    }
}
