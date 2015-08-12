import QtQuick 2.4
import QtQuick.Window 2.2

Window {
    width: 800
    height: 480

    Component.onCompleted: {
        Qt.application.name = "quickews";
        Qt.application.organization = "quickews";
        loader.active = true;
    }

    Loader {
        id: loader
        anchors.fill: parent
        active: false
        source: "MainLayout.qml"
    }
}
