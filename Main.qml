import QtQuick.Controls
import QtCharts
import QtQml

ApplicationWindow {
    visible: true

    title: qsTr("micfft")
    width: 640
    height: 480

    ChartView {
        id: chartView
        title: "Microphone"
        legend.alignment: Qt.AlignCenter
        anchors.fill: parent
        antialiasing: true
        axes: [
            ValueAxis {
                id: xAxis
                min: -100
                max: device.rate / 2
            },
            ValueAxis {
                id: yAxis
                min: -100
                max: 200
            }
        ]

        LineSeries {
            id: lineSeries
            axisX: xAxis
            axisY: yAxis
        }
    }

    Timer {
        interval: 200
        running: true
        repeat: true
        onTriggered: {
            lineSeries.clear();
            const points = device.points;
            for (var i = 0; i < device.frames; i++) {
                lineSeries.append(points[i].x, points[i].y);
            }
        }
    }
}
