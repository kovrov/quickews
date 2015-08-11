#include "parser.h"

#define MATCH(LITERAL) compare(QStringLiteral(LITERAL), Qt::CaseInsensitive) == 0

namespace parser {

void body(QXmlStreamReader &xml, ews::AbstractResponse *res);
void calendarEvent(QXmlStreamReader &xml, ews::GetUserAvailabilityResponse *res);
void calendarEventArray(QXmlStreamReader &xml, ews::GetUserAvailabilityResponse *res);
void calendarFolder(QXmlStreamReader &xml, ews::Folder *res);
void calendarItem(QXmlStreamReader &xml, ews::FindItemResponse *res);
void end(QXmlStreamReader &xml, ews::CalendarItem *item);
void endTime(QXmlStreamReader &xml, ews::CalendarEvent *res);
void envelope(QXmlStreamReader &xml, ews::AbstractResponse *res);
void findItemResponse(QXmlStreamReader &xml, ews::FindItemResponse *res);
void findItemResponseMessage(QXmlStreamReader &xml, ews::FindItemResponse *res);
void folder(QXmlStreamReader &xml, ews::Folder *res);
void folders(QXmlStreamReader &xml, ews::GetFolderResponse *res);
void freeBusyResponse(QXmlStreamReader &xml, ews::GetUserAvailabilityResponse *res);
void freeBusyResponseArray(QXmlStreamReader &xml, ews::GetUserAvailabilityResponse *res);
void freeBusyView(QXmlStreamReader &xml, ews::GetUserAvailabilityResponse *res);
void getFolderResponse(QXmlStreamReader &xml, ews::GetFolderResponse *res);
void getFolderResponseMessage(QXmlStreamReader &xml, ews::GetFolderResponse *res);
void getUserAvailabilityResponse(QXmlStreamReader &xml, ews::GetUserAvailabilityResponse *res);
void items(QXmlStreamReader &xml, ews::FindItemResponse *res);
void location(QXmlStreamReader &xml, ews::CalendarItem *item);
void responseCode(QXmlStreamReader &xml, ews::AbstractResponse *res);
void responseMessage(QXmlStreamReader &xml, ews::GetUserAvailabilityResponse *res);
void responseMessages(QXmlStreamReader &xml, ews::AbstractResponse *res);
void rootFolder(QXmlStreamReader &xml, ews::FindItemResponse *res);
void start(QXmlStreamReader &xml, ews::CalendarItem *item);
void startTime(QXmlStreamReader &xml, ews::CalendarEvent *res);
void subject(QXmlStreamReader &xml, ews::CalendarItem *item);

} // namespace parser

ews::GetFolderResponse::GetFolderResponse(const QByteArray &data)
{
    QXmlStreamReader xml(data);
    Q_ASSERT (xml.tokenType() == QXmlStreamReader::NoToken);
    xml.readNextStartElement();
    if (xml.name() == QStringLiteral("Envelope")) {
        parser::envelope(xml, this);
    }
}

ews::GetUserAvailabilityResponse::GetUserAvailabilityResponse(const QByteArray &data)
{
    QXmlStreamReader xml(data);
    Q_ASSERT (xml.tokenType() == QXmlStreamReader::NoToken);
    xml.readNextStartElement();
    if (xml.name() == QStringLiteral("Envelope")) {
        parser::envelope(xml, this);
    }
}

namespace parser {

void body(QXmlStreamReader &xml, ews::AbstractResponse *res)
{
    Q_ASSERT (xml.name().MATCH("Body"));
    while (xml.readNextStartElement()) {
        if (xml.name().MATCH("FindItemResponse")) {
            findItemResponse(xml, dynamic_cast<ews::FindItemResponse*>(res));
        } else if (xml.name().MATCH("GetFolderResponse")) {
            getFolderResponse(xml, dynamic_cast<ews::GetFolderResponse*>(res));
        } else if (xml.name().MATCH("GetUserAvailabilityResponse")) {
            getUserAvailabilityResponse(xml, dynamic_cast<ews::GetUserAvailabilityResponse*>(res));
        } else {
            xml.skipCurrentElement();
        }
    }
}

void calendarEvent(QXmlStreamReader &xml, ews::GetUserAvailabilityResponse *res)
{
    Q_ASSERT (xml.name().MATCH("CalendarEvent"));
    ews::CalendarEvent event;
    while (xml.readNextStartElement()) {
        if (xml.name().MATCH("StartTime")) {
            startTime(xml, &event);
        } else if (xml.name().MATCH("EndTime")) {
            endTime(xml, &event);
        } else {
            xml.skipCurrentElement();
        }
    }
    res->events << event;
}

void calendarEventArray(QXmlStreamReader &xml, ews::GetUserAvailabilityResponse *res)
{
    Q_ASSERT (xml.name().MATCH("CalendarEventArray"));
    while (xml.readNextStartElement()) {
        if (xml.name().MATCH("CalendarEvent")) {
            calendarEvent(xml, res);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void calendarFolder(QXmlStreamReader &xml, ews::Folder *res)
{
    Q_ASSERT (xml.name().MATCH("CalendarFolder"));
    while (xml.readNextStartElement()) {
        if (xml.name().MATCH("FolderId")) {
            res->folderId = xml.attributes().value("Id").toString();
        } else {
            xml.skipCurrentElement();
        }
    }
}

void calendarItem(QXmlStreamReader &xml, ews::FindItemResponse *res)
{
    ews::CalendarItem item;
    Q_ASSERT (xml.name().MATCH("CalendarItem"));
    while (xml.readNextStartElement()) {
        if (xml.name().MATCH("Subject")) {
            subject(xml, &item);
        } else if (xml.name().MATCH("Start")) {
            start(xml, &item);
        } else if (xml.name().MATCH("End")) {
            end(xml, &item);
        } else if (xml.name().MATCH("Location")) {
            location(xml, &item);
        } else {
            xml.skipCurrentElement();
        }
    }
    res->calendarItems << item;
}

void end(QXmlStreamReader &xml, ews::CalendarItem *item)
{
    Q_ASSERT (xml.name().MATCH("End"));
    item->end = QDateTime::fromString(xml.readElementText(), Qt::ISODate);
    if (!xml.isEndElement()) {
        xml.skipCurrentElement();
    }
}

void endTime(QXmlStreamReader &xml, ews::CalendarEvent *res)
{
    Q_ASSERT (xml.name().MATCH("EndTime"));
    res->endTime = QDateTime::fromString(xml.readElementText(), Qt::ISODate);
    if (!xml.isEndElement()) {
        xml.skipCurrentElement();
    }
}

void envelope(QXmlStreamReader &xml, ews::AbstractResponse *res)
{
    Q_ASSERT (xml.name().MATCH("Envelope"));
    while (xml.readNextStartElement()) {
        if (xml.name().MATCH("Body")) {
            body(xml, res);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void findItemResponse(QXmlStreamReader &xml, ews::FindItemResponse *res)
{
    Q_ASSERT (xml.name().MATCH("FindItemResponse"));
    while (xml.readNextStartElement()) {
        if (xml.name().MATCH("ResponseMessages")) {
            responseMessages(xml, res);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void findItemResponseMessage(QXmlStreamReader &xml, ews::FindItemResponse *res)
{
    Q_ASSERT (xml.name().MATCH("FindItemResponseMessage"));
    while (xml.readNextStartElement()) {
        if (xml.name().MATCH("RootFolder")) {
            rootFolder(xml, res);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void folder(QXmlStreamReader &xml, ews::Folder *res)
{
    Q_ASSERT (xml.name().MATCH("Folder"));
    while (xml.readNextStartElement()) {
        if (xml.name().MATCH("FolderId")) {
            res->folderId = xml.attributes().value("Id").toString();
        } else {
            xml.skipCurrentElement();
        }
    }
}

void folders(QXmlStreamReader &xml, ews::GetFolderResponse *res)
{
    Q_ASSERT (xml.name().MATCH("Folders"));
    while (xml.readNextStartElement()) {
        if (xml.name().MATCH("Folder")) {
            ews::Folder f;
            folder(xml, &f);
            res->folders << f;
        } else if (xml.name().MATCH("CalendarFolder")) {
            ews::Folder f;
            calendarFolder(xml, &f);
            res->folders << f;
        } else {
            xml.skipCurrentElement();
        }
    }
}

void freeBusyResponse(QXmlStreamReader &xml, ews::GetUserAvailabilityResponse *res)
{
    Q_ASSERT (xml.name().MATCH("FreeBusyResponse"));
    while (xml.readNextStartElement()) {
        if (xml.name().MATCH("ResponseMessage")) {
            responseMessage(xml, res);
        } else if (xml.name().MATCH("FreeBusyView")) {
            freeBusyView(xml, res);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void freeBusyResponseArray(QXmlStreamReader &xml, ews::GetUserAvailabilityResponse *res)
{
    Q_ASSERT (xml.name().MATCH("FreeBusyResponseArray"));
    while (xml.readNextStartElement()) {
        if (xml.name().MATCH("FreeBusyResponse")) {
            freeBusyResponse(xml, res);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void freeBusyView(QXmlStreamReader &xml, ews::GetUserAvailabilityResponse *res)
{
    Q_ASSERT (xml.name().MATCH("FreeBusyView"));
    while (xml.readNextStartElement()) {
        if (xml.name().MATCH("CalendarEventArray")) {
            calendarEventArray(xml, res);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void getFolderResponse(QXmlStreamReader &xml, ews::GetFolderResponse *res)
{
    Q_ASSERT (xml.name().MATCH("GetFolderResponse"));
    while (xml.readNextStartElement()) {
        if (xml.name().MATCH("ResponseMessages")) {
            responseMessages(xml, res);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void getFolderResponseMessage(QXmlStreamReader &xml, ews::GetFolderResponse *res)
{
    Q_ASSERT (xml.name().MATCH("GetFolderResponseMessage"));
    while (xml.readNextStartElement()) {
        if (xml.name().MATCH("ResponseCode")) {
            responseCode(xml, res);
        } else if (xml.name().MATCH("Folders")) {
            folders(xml, res);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void getUserAvailabilityResponse(QXmlStreamReader &xml, ews::GetUserAvailabilityResponse *res)
{
    Q_ASSERT (xml.name().MATCH("GetUserAvailabilityResponse"));
    while (xml.readNextStartElement()) {
        if (xml.name().MATCH("FreeBusyResponseArray")) {
            freeBusyResponseArray(xml, res);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void items(QXmlStreamReader &xml, ews::FindItemResponse *res)
{
    Q_ASSERT (xml.name().MATCH("Items"));
    while (xml.readNextStartElement()) {
        if (xml.name().MATCH("CalendarItem")) {
            calendarItem(xml, res);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void location(QXmlStreamReader &xml, ews::CalendarItem *item)
{
    Q_ASSERT (xml.name().MATCH("Location"));
    item->location = xml.readElementText();
    if (!xml.isEndElement()) {
        xml.skipCurrentElement();
    }
}

void responseCode(QXmlStreamReader &xml, ews::AbstractResponse *res)
{
    Q_ASSERT (xml.name().MATCH("ResponseCode"));
    auto responseCode = xml.readElementText();
    res->response = ews::responseCodes.value(responseCode);

    if (!xml.isEndElement()) {
        xml.skipCurrentElement();
    }
}

void responseMessage(QXmlStreamReader &xml, ews::GetUserAvailabilityResponse *res)
{
    Q_ASSERT (xml.name().MATCH("ResponseMessage"));
    while (xml.readNextStartElement()) {
        if (xml.name().MATCH("ResponseCode")) {
            responseCode(xml, res);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void responseMessages(QXmlStreamReader &xml, ews::AbstractResponse *res)
{
    Q_ASSERT (xml.name().MATCH("ResponseMessages"));
    while (xml.readNextStartElement()) {
        if (xml.name().MATCH("FindItemResponseMessage")) {
            auto r = dynamic_cast<ews::FindItemResponse*>(res);
            findItemResponseMessage(xml, r);
        } else if (xml.name().MATCH("GetFolderResponseMessage")) {
            auto r = dynamic_cast<ews::GetFolderResponse*>(res);
            getFolderResponseMessage(xml, r);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void rootFolder(QXmlStreamReader &xml, ews::FindItemResponse *res)
{
    Q_ASSERT (xml.name().MATCH("RootFolder"));
    while (xml.readNextStartElement()) {
        if (xml.name().MATCH("Items")) {
            items(xml, res);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void start(QXmlStreamReader &xml, ews::CalendarItem *item)
{
    Q_ASSERT (xml.name().MATCH("Start"));
    item->start = QDateTime::fromString(xml.readElementText(), Qt::ISODate);
    if (!xml.isEndElement()) {
        xml.skipCurrentElement();
    }
}

void startTime(QXmlStreamReader &xml, ews::CalendarEvent *res)
{
    Q_ASSERT (xml.name().MATCH("StartTime"));
    res->startTime = QDateTime::fromString(xml.readElementText(), Qt::ISODate);
    if (!xml.isEndElement()) {
        xml.skipCurrentElement();
    }
}

void subject(QXmlStreamReader &xml, ews::CalendarItem *item)
{
    Q_ASSERT (xml.name().MATCH("Subject"));
    item->subject = xml.readElementText();
    if (!xml.isEndElement()) {
        xml.skipCurrentElement();
    }
}

} // namespace parser
