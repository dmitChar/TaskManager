import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    height: 50
    color: "#34495e"


    RowLayout {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 30

        Label {
            text: "Сегодня: " + taskController.todayDone
            color: "white"

        }

        Label {
            text: "За 7 дней: " + taskController.weekDone
            color: "white"
        }

        Label {
            text: "Среднее время: " +
                  taskController.avgCompletionTime + " ч"
            color: "white"
        }
    }
}
