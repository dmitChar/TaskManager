import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle
{
    height: 60
    color: "#2c3e50"
    Layout.fillWidth: true

    RowLayout
    {
        anchors.fill: parent
        anchors.margins: 10
        spacing: 10

        TextField
        {
            id: searchField
            placeholderText: "Поиск по названию..."
            Layout.fillWidth: true
            onTextChanged: taskController.setSearchText(text)
        }

        ComboBox
        {
            model: ["all", "work", "study", "urgent"]
            onCurrentTextChanged: taskController.setTagFilter(currentText)
        }

        Button
        {
            text: "Экспорт в JSON"
            onClicked: taskController.exportJson()
        }
    }

}
