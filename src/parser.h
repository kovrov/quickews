#ifndef PARSER_H
#define PARSER_H

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QXmlStreamReader>

#include "errors.h"

namespace ews {

    struct AbstractResponse
    {
        ews::ResponseCode response;
        virtual ~AbstractResponse() = default;
    };

    struct CalendarEvent
    {
        QDateTime startTime;
        QDateTime endTime;
    };

    struct CalendarItem
    {
        QString id;
        QString subject;
        QDateTime start;
        QDateTime end;
        QString location;
    };

    struct FindItemResponse : public AbstractResponse
    {
        QList<CalendarItem> calendarItems;
    };

    struct Folder
    {
        QString folderId;
    };

    struct GetFolderResponse : public AbstractResponse
    {
        GetFolderResponse(const QByteArray &data);
        QList<Folder> folders;
    };

    struct GetUserAvailabilityResponse : public AbstractResponse
    {
        GetUserAvailabilityResponse(const QByteArray &data);
        QList<CalendarEvent> events;
    };

} // namespace ews

#endif // PARSER_H
