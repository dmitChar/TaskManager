import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle
{
    id: card
    width: ListView.view ? ListView.view.width : 200
    height: 150
    radius: 8
    color: "white"
    border.color: dragArea.drag.active ? "#3498db" : "#e0e0e0"
    border.width: dragArea.drag.active ? 2 : 1

    property string dragTaskId
    property string dragFromStatus
    property string title
    property string description
    property int priority
    property var tags

    signal editRequested(int taskId)

    Drag.active: dragArea.drag.active
    Drag.hotSpot.x: width / 2
    Drag.hotSpot.y: height / 2
    Drag.keys: ["task"]
    Drag.source: card

    states: [
        State {
            name: "dragging"
            when: dragArea.drag.active

            PropertyChanges {
                target: card
                scale: 1.05
                z: 999
                opacity: 0.85
                border.color: "#3498db"
                border.width: 2
            }
        }
    ]

    transitions: Transition {
        NumberAnimation {
            properties: "scale, opacity"
            duration: 120
            easing.type: Easing.OutCubic
        }
    }

    // Полоса приоритета слева
    Rectangle {
        id: priorityBar
        width: 4
        height: parent.height
        anchors.left: parent.left
        radius: 12
        color: {
            if (priority === 1) return "#e74c3c"  // Высокий - красный
            if (priority === 2) return "#f39c12"  // Средний - оранжевый
            if (priority === 3) return "#2ecc71"  // Низкий - зеленый
            return "#bdc3c7"  // Без приоритета - серый
        }
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        anchors.leftMargin: 20
        spacing: 8

        // Заголовок с тегами
        RowLayout {
            Layout.fillWidth: true
            spacing: 8

            Text {
                id: titleText
                Layout.fillWidth: true
                text: title
                font.bold: true
                font.pixelSize: 14
                elide: Text.ElideRight
                color: "#2c3e50"
                wrapMode: Text.WordWrap
                maximumLineCount: 2
            }

            // Индикатор приоритета (иконка)
            Rectangle {
                width: 24
                height: 24
                radius: 12
                visible: priority !== undefined && priority > 0
                color: {
                    if (priority === 1) return "#fee"
                    if (priority === 2) return "#fef5e7"
                    if (priority === 3) return "#eafaf1"
                    return "transparent"
                }

                Text {
                    anchors.centerIn: parent
                    text: {
                        if (priority === 1) return "!"
                        if (priority === 2) return "↑"
                        if (priority === 3) return "↓"
                        return ""
                    }
                    font.bold: true
                    font.pixelSize: 14
                    color: {
                        if (priority === 1) return "#e74c3c"
                        if (priority === 2) return "#f39c12"
                        if (priority === 3) return "#2ecc71"
                        return "#bdc3c7"
                    }
                }
            }
        }

        // Описание
        Text {
            Layout.fillWidth: true
            text: description || ""
            visible: description !== undefined && description !== ""
            font.pixelSize: 12
            color: "#7f8c8d"
            elide: Text.ElideRight
            maximumLineCount: 2
            wrapMode: Text.WordWrap
        }

        Item {
            Layout.fillHeight: true
        }

        // Теги внизу
        Flow {
            Layout.fillWidth: true
            spacing: 6
            visible: tags !== undefined && tags.length > 0

            Repeater {
                model: tags
                delegate: Rectangle {
                    height: 22
                    width: tagText.implicitWidth + 16
                    radius: 11

                    color: {
                        if (modelData === "work") return "#3498db"
                        if (modelData === "study") return "#2ecc71"
                        if (modelData === "urgent") return "#e74c3c"
                        return "#95a5a6"
                    }

                    Text {
                        id: tagText
                        anchors.centerIn: parent
                        text: modelData
                        color: "white"
                        font.pixelSize: 10
                        font.bold: true
                    }
                }
            }
        }
    }

    MouseArea {
        id: dragArea
        anchors.fill: parent
        drag.target: card
        drag.axis: Drag.XAndYAxis
        cursorShape: Qt.PointingHandCursor
        hoverEnabled: true
        preventStealing: true
        acceptedButtons: Qt.LeftButton

        onEntered: {
            if (!drag.active) {
                card.border.color = "#bdc3c7"
                cursorShape = Qt.OpenHandCursor
            }
        }
        onDoubleClicked:
        {
            card.editRequested(model.id)
        }

        onExited: {
            if (!drag.active) {
                card.border.color = "#e0e0e0"
            }
        }

        onPressed: {
            cursorShape = Qt.ClosedHandCursor
        }

        onReleased: {
            cursorShape = Qt.OpenHandCursor
            card.Drag.drop()
        }
    }
}
