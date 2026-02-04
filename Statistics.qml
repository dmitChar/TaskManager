import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtCharts


Item
{
    readonly property int periodToday: 0
    readonly property int periodWeek: 1
    readonly property int periodMonth: 2
    readonly property int periodYear: 3

    property int currentPeriod: periodToday

    Component.onCompleted: {
            reloadCharts()
        }
    ColumnLayout
    {
        anchors.fill: parent
        RowLayout
        {
            spacing: 8
            Layout.fillWidth: true

            Repeater
            {
                model: [
                    { text: "Today", value: periodToday },
                    { text: "Week", value: periodWeek },
                    { text: "Month", value: periodMonth },
                    { text: "Year", value: periodYear }
                ]
                Button
                {
                    id: periodButton
                    text:modelData.text
                    checkable: true
                    Layout.fillWidth: true
                    implicitHeight: 55
                    checked: index === 0
                    ButtonGroup.group: periodGroup


                    contentItem: Text
                    {
                        text: periodButton.text
                        font.pixelSize: 14
                        font.bold: periodButton.checked
                        color: periodButton.checked ? "white" : "#2c3e50"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }

                    background: Rectangle
                    {
                        radius:10
                        color: {
                            if (periodButton.checked) return "#3498db"
                            if (periodButton.hovered) return "#ecf0f1"
                            return "white"
                        }
                        border.color: periodButton.checked ? "#3498db" : "#dfe4ea"
                        border.width: periodButton.checked ? 2 : 1

                        Behavior on color {
                            ColorAnimation { duration: 150 }
                        }

                        Behavior on border.color {
                            ColorAnimation { duration: 150 }
                        }
                    }
                    onClicked:
                    {
                        console.log("Choosed button:", modelData.value)
                        currentPeriod = modelData.value
                        reloadCharts()
                    }
                }
            }
        }
        ButtonGroup
        {
            id: periodGroup
        }

        //Грид графиков
        GridLayout
        {
            Layout.fillHeight: true
            Layout.fillWidth: true
            columns: 3
            rowSpacing: 20
            columnSpacing: 20


            //Completed tasks
            ChartView {
                width: 400
                height: 300
                antialiasing: true
                    animationOptions: ChartView.SeriesAnimations
                    title: "Completed Tasks"
                    titleFont.pixelSize: 16
                    titleFont.bold: true
                    theme: ChartView.ChartThemeLight
                    legend.visible: false

                BarCategoryAxis
                {
                    id: completedAxisX
                    categories: []
                }

                ValueAxis
                {
                    id: completedAxisY
                    min: 0
                }

                BarSeries
                {
                    id: completedSeries
                    axisX: completedAxisX
                    axisY: completedAxisY
                }
            }


        }

    }

    function reloadCharts()
    {
        loadCompleted()
    }

    function loadCompleted()
    {
        completedSeries.clear()
        completedAxisX.categories = []

        const data = taskController.getCompletedTasks(currentPeriod)
        console.log("Received data:", JSON.stringify(data))

        if (!data || data.length === 0)
            return

        let categories = []
        let values = []
        let maxValue = 0

        for (let i = 0; i < data.length; i++)
        {
            categories.push(data[i].label)
            values.push(data[i].value)
            maxValue = Math.max(maxValue, data[i].value)
        }

        completedAxisX.categories = categories

        let barSet = completedSeries.append("Done", values)

        if (!barSet)
        {
            console.error("Failed to create BarSet!")
            return
        }
        completedAxisY.max = maxValue > 0 ? Math.ceil(maxValue * 1.2) : 10
    }

}
