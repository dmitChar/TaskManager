import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts


Window
{
    width: 1200
    height: 700
    visible: true
    title: "Task Manager"

    ColumnLayout
    {
        anchors.fill: parent
        spacing: 8

        TopBar {}
        Board
        {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Footer
        {
            Layout.fillWidth: true
        }

    }

}
