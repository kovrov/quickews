#ifndef PARSER_H
#define PARSER_H

#include <QtCore/QString>
#include <QtCore/QDateTime>
#include <QtCore/QXmlStreamReader>

#include "errors.h"

namespace ews {

    struct CalendarItem
    {
        QString id;
        QString subject;
        QDateTime start;
        QDateTime end;
        QString location;
    };

    struct Folder
    {
        QString folderId;
    };

    struct AbstractResponse
    {
        ews::ResponseCode response;
        virtual ~AbstractResponse() = default;
    };

    struct FindItemResponse : public AbstractResponse
    {
        QList<CalendarItem> calendarItems;
    };

    struct GetFolderResponse : public AbstractResponse
    {
        GetFolderResponse(const QByteArray &data);
        QList<Folder> folders;
    };
} // namespace ews

#endif // PARSER_H
