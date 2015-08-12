import QtQuick 2.4

QtObject {

    property date date;

    property int resolution: 60 * 1000 // 1 minute

    property Timer timer: Timer {
        id: timer

        Component.onCompleted: {
            var d = new Date();
            triggered.connect(align);
            interval = resolution - (d.getTime() % resolution);
            start();
            date = d;
        }

        function align() {
            triggered.disconnect(align);
            triggered.connect(tick);
            interval = resolution;
            repeat = true;
            triggeredOnStart = true;
            restart();
        }

        function tick() {
            date = new Date();
        }
    }
}
