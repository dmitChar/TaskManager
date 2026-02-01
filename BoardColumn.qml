import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: column
    required property string title
    required property string status

    radius: 10
    color: dropArea.containsDrag ? "#d5dbdb" : "#ecf0f1"
    border.color: dropArea.containsDrag ? "#3498db" : "transparent"
    border.width: 2
    Layout.fillHeight: true
    Layout.fillWidth: true

    Behavior on color {
        ColorAnimation { duration: 200 }
    }

    //Окно редактирования задачи
    EditWindow {
        id: editWindow
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 6

        // Заголовок с кнопкой сортировки
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Label {
                text: title
                font.bold: true
                font.pointSize: 14
                color: "#2c3e50"
                Layout.fillWidth: true
            }

            // Кнопка сортировки по приоритету
            Button {
                id: sortButton
                width: 32
                height: 32

                property int sortState: 0 // 0 = NoSort, 1 = HighFirst, 2 = LowFirst

                contentItem: Text {
                    text: {
                        if (sortButton.sortState === 1) return "↓" // Высокий сначала (1->2->3)
                        if (sortButton.sortState === 2) return "↑" // Низкий сначала (3->2->1)
                        return "⇅" // Без сортировки
                    }
                    font.pixelSize: 16
                    font.bold: true
                    color: sortButton.sortState === 0 ? "#95a5a6" : "#3498db"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                background: Rectangle {
                    radius: 6
                    color: sortButton.hovered ? "#ecf0f1" : "transparent"
                    border.color: sortButton.sortState === 0 ? "transparent" : "#3498db"
                    border.width: 1

                    Behavior on color {
                        ColorAnimation { duration: 150 }
                    }
                }

                ToolTip.visible: hovered
                ToolTip.delay: 500
                ToolTip.text: {
                    if (sortButton.sortState === 0) return "No sorting"
                    if (sortButton.sortState === 1) return "High priority first"
                    return "Low priority first"
                }

                onClicked: {
                    sortState = (sortState + 1) % 3
                    taskController.setPrioritySort(column.status, sortState)
                }

                Component.onCompleted: {
                    sortState = taskController.getPrioritySort(column.status)
                }
            }
        }


        ListView
        {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 6
            clip: true
            model: taskController.modelForStatus(status)
            delegate: TaskCard
            {
                dragTaskId: model.id
                dragFromStatus: model.status
                title: model.title
                description: model.description
                priority: model.priority
                tags: model.tags

                onEditRequested: function(id)
                {
                    editWindow.openEditWindow(model.id, dragFromStatus)
                }
            }

            add: Transition {
                NumberAnimation { property: "opacity"; from: 0; to: 1; duration: 200 }
                NumberAnimation { property: "y"; from: y + 20; duration: 200 }
            }

            remove: Transition {
                NumberAnimation { property: "opacity"; to: 0; duration: 150 }
            }

            move: Transition {
                NumberAnimation {
                    properties: "x,y"
                    duration: 200
                    easing.type: Easing.OutCubic
                }
            }

            displaced: Transition {
                NumberAnimation {
                    properties: "x,y"
                    duration: 200
                    easing.type: Easing.OutCubic
                }
            }

            // Placeholder когда колонка пустая
            Label {
                anchors.centerIn: parent
                visible: listView.count === 0
                text: "Drop tasks here"
                color: "#95a5a6"
                font.italic: true
                z: -1
            }
        }
    }

    DropArea {
        id: dropArea
        anchors.fill: parent
        keys: ["task"]

        onEntered: function(drag) {
            console.log("Entered column:", column.status)
        }

        onExited: {
            console.log("Exited column:", column.status)
        }

        onDropped: function(drop) {
            console.log("Drop received in column:", column.status)

            if (drop.keys.indexOf("task") !== -1 && drop.source) {
                const taskId = drop.source.dragTaskId
                const fromStatus = drop.source.dragFromStatus

                console.log("Task:", taskId, "from:", fromStatus, "to:", column.status)

                if (fromStatus !== undefined && taskId !== undefined)
                {
                    // Перемещение между колонками - всегда вставляем в начало (индекс 0)
                    console.log("Moving to top of column")
                    taskController.moveTask(parseInt(taskId), column.status)
                    drop.accept()

                }
                else {
                    console.error("Invalid drop data")
                    drop.ignore()
                }
            }
        }
    }
}
