import QtQuick 2.4
import Qt.labs.settings 1.0

import EWS 1.0 as Exchange

Item {

    Settings {
        id: settings
        property string mailbox
    }

    Clock {
        id: clock
    }

    Exchange.UserAvailabilityModel {
        id: availabilityModel
        query: {
            var start = new Date(clock.date.getTime());
            start.setHours(0);
            start.setMinutes(0);
            start.setMilliseconds(0);
            start.setSeconds(0);
            var end = new Date(start.getTime());
            end.setDate(end.getDate() + 1);
            return {
                mailbox: settings.mailbox,
                startDate: start,
                endDate: end
            };
        }
    }

    RoomStatus {
        id: room
        dateTime: clock.date
        model: availabilityModel
    }

    Rectangle {
        anchors.fill: parent
        width: 128; height: 64
        color: room.available ? "#8f8" : "#f9a"
    }

    Text {
        anchors.centerIn: parent
        text: room.statusText
        font.pixelSize: 32
    }
}
