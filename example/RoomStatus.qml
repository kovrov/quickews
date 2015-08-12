import QtQuick 2.4

QtObject{

    property date dateTime

    property var model

    readonly property bool available: isNaN(nextAvailableSlot.start) || nextAvailableSlot.start < dateTime

    readonly property var nextAvailableSlot: {
        if (!dateTime.getTime()) {
            return; // clock not ready
        }
        var slot = { start: NaN, end: NaN };
        for (var i = 0; i < model.count; ++i) {
            var event = model.get(i);
            slot.end = event.startTime.getTime();
            if (!((slot.end - slot.start) < (5 * 60 * 1000))) {
                if ((dateTime > slot.start || isNaN(slot.start)) && (dateTime < slot.end || isNaN(slot.end))) {
                    return slot;
                } else if (dateTime < slot.start) {
                    return slot;
                }
            }
            slot.start = event.endTime.getTime();
        }
        slot.end = NaN;
        return slot;
    }

    readonly property string statusText: {
        if (available) {
            return nextAvailableSlot.end ?
                    "Available for next " +
                    Math.ceil((nextAvailableSlot.end - dateTime) / 1000 / 60) +
                    " minutes." :
                    "Available till the rest of the day.";
        } else {
            return "Booked for next " +
                    Math.ceil((nextAvailableSlot.start - dateTime) / 1000 / 60) +
                    " minutes.";
        }
    }
}
