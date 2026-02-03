import QtQuick
import QtQuick.Layouts

ColumnLayout
{
    RowLayout
    {
        Layout.fillHeight: true
        Layout.fillWidth: true
        spacing: 8

        BoardColumn { title: "Backlog"; status: "backlog" }
        BoardColumn { title: "In Progress"; status: "progress" }
        BoardColumn { title: "Review"; status: "review" }
        BoardColumn { title: "Done"; status: "done" }
    }
    Footer { Layout.fillWidth: true }
}

