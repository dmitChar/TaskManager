import QtQuick
import QtQuick.Layouts

RowLayout
{
    Layout.fillHeight: true
    Layout.fillWidth: true
    spacing: 8
    BoardColumn{title: "Backlog"; status: "backlog" }
    BoardColumn { title: "In Progress"; status: "progress" }
    BoardColumn { title: "Done"; status: "done" }
}

