#include "parser.h"

#define MATCH(STR, LIT) STR.compare(QStringLiteral(LIT), Qt::CaseInsensitive) == 0

namespace parser {

void envelope(QXmlStreamReader &xml, ews::AbstractResponse *res);
void body(QXmlStreamReader &xml, ews::AbstractResponse *res);
void findItemResponse(QXmlStreamReader &xml, ews::FindItemResponse *res);
void getFolderResponse(QXmlStreamReader &xml, ews::GetFolderResponse *res);
void responseMessages(QXmlStreamReader &xml, ews::AbstractResponse *res);
void findItemResponseMessage(QXmlStreamReader &xml, ews::FindItemResponse *res);
void getFolderResponseMessage(QXmlStreamReader &xml, ews::GetFolderResponse *res);
void responseCode(QXmlStreamReader &xml, ews::AbstractResponse *res);
void folders(QXmlStreamReader &xml, ews::GetFolderResponse *res);
void folder(QXmlStreamReader &xml, ews::Folder *res);
void calendarFolder(QXmlStreamReader &xml, ews::Folder *res);
void rootFolder(QXmlStreamReader &xml, ews::FindItemResponse *res);
void items(QXmlStreamReader &xml, ews::FindItemResponse *res);
void calendarItem(QXmlStreamReader &xml, ews::FindItemResponse *res);
void subject(QXmlStreamReader &xml, ews::CalendarItem *item);
void start(QXmlStreamReader &xml, ews::CalendarItem *item);
void end(QXmlStreamReader &xml, ews::CalendarItem *item);
void location(QXmlStreamReader &xml, ews::CalendarItem *item);

} // namespace parser

ews::GetFolderResponse::GetFolderResponse(const QByteArray &data)
{
    QXmlStreamReader xml(data);
    Q_ASSERT (xml.tokenType() == QXmlStreamReader::NoToken);
    xml.readNextStartElement();
    if (xml.name() == QStringLiteral("Envelope"))
        parser::envelope(xml, this);
}

namespace parser {

void envelope(QXmlStreamReader &xml, ews::AbstractResponse *res)
{
    Q_ASSERT (xml.name().compare(QStringLiteral("Envelope"), Qt::CaseInsensitive) == 0);
    while (xml.readNextStartElement()) {
        if (xml.name().compare(QStringLiteral("Body"), Qt::CaseInsensitive) == 0)
            body(xml, res);
        else
            xml.skipCurrentElement();
    }
}

void body(QXmlStreamReader &xml, ews::AbstractResponse *res)
{
    Q_ASSERT (xml.name().compare(QStringLiteral("Body"), Qt::CaseInsensitive) == 0);
    while (xml.readNextStartElement()) {
        if (xml.name().compare(QStringLiteral("FindItemResponse"), Qt::CaseInsensitive) == 0) {
            auto findItemResp = dynamic_cast<ews::FindItemResponse*>(res);
            findItemResponse(xml, findItemResp);
        } else if (xml.name().compare(QStringLiteral("GetFolderResponse"), Qt::CaseInsensitive) == 0) {
            auto getFolderResp = dynamic_cast<ews::GetFolderResponse*>(res);
            getFolderResponse(xml, getFolderResp);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void findItemResponse(QXmlStreamReader &xml, ews::FindItemResponse *res)
{
    Q_ASSERT (xml.name().compare(QStringLiteral("FindItemResponse"), Qt::CaseInsensitive) == 0);
    while (xml.readNextStartElement()) {
        if (xml.name().compare(QStringLiteral("ResponseMessages"), Qt::CaseInsensitive) == 0)
            responseMessages(xml, res);
        else
            xml.skipCurrentElement();
    }
}

void getFolderResponse(QXmlStreamReader &xml, ews::GetFolderResponse *res)
{
    Q_ASSERT (xml.name().compare(QStringLiteral("GetFolderResponse"), Qt::CaseInsensitive) == 0);
    while (xml.readNextStartElement()) {
        if (xml.name().compare(QStringLiteral("ResponseMessages"), Qt::CaseInsensitive) == 0)
            responseMessages(xml, res);
        else
            xml.skipCurrentElement();
    }
}

void responseMessages(QXmlStreamReader &xml, ews::AbstractResponse *res)
{
    Q_ASSERT (xml.name().compare(QStringLiteral("ResponseMessages"), Qt::CaseInsensitive) == 0);
    while (xml.readNextStartElement()) {
        if (xml.name().compare(QStringLiteral("FindItemResponseMessage"), Qt::CaseInsensitive) == 0) {
            auto r = dynamic_cast<ews::FindItemResponse*>(res);
            findItemResponseMessage(xml, r);
        } else if (xml.name().compare(QStringLiteral("GetFolderResponseMessage"), Qt::CaseInsensitive) == 0) {
            auto r = dynamic_cast<ews::GetFolderResponse*>(res);
            getFolderResponseMessage(xml, r);
        } else {
            xml.skipCurrentElement();
        }
    }
}

void findItemResponseMessage(QXmlStreamReader &xml, ews::FindItemResponse *res)
{
    Q_ASSERT (xml.name().compare(QStringLiteral("FindItemResponseMessage"), Qt::CaseInsensitive) == 0);
    while (xml.readNextStartElement()) {
        if (xml.name().compare(QStringLiteral("RootFolder"), Qt::CaseInsensitive) == 0)
            rootFolder(xml, res);
        else
            xml.skipCurrentElement();
    }
}

void getFolderResponseMessage(QXmlStreamReader &xml, ews::GetFolderResponse *res)
{
    Q_ASSERT (xml.name().compare(QStringLiteral("GetFolderResponseMessage"), Qt::CaseInsensitive) == 0);
    while (xml.readNextStartElement()) {
        if (MATCH(xml.name(), "ResponseCode"))
            responseCode(xml, res);
        else if (xml.name().compare(QStringLiteral("Folders"), Qt::CaseInsensitive) == 0)
            folders(xml, res);
        else
            xml.skipCurrentElement();
    }
}

#include "errors.h"

void responseCode(QXmlStreamReader &xml, ews::AbstractResponse *res)
{
    Q_ASSERT (xml.name().compare(QStringLiteral("ResponseCode"), Qt::CaseInsensitive) == 0);
    auto responseCode = xml.readElementText();
    res->response = ews::responseCodes.value(responseCode);

    if (!xml.isEndElement())
        xml.skipCurrentElement();
}

void folders(QXmlStreamReader &xml, ews::GetFolderResponse *res)
{
    Q_ASSERT (xml.name().compare(QStringLiteral("Folders"), Qt::CaseInsensitive) == 0);
    while (xml.readNextStartElement()) {
        if (xml.name().compare(QStringLiteral("Folder"), Qt::CaseInsensitive) == 0) {
            ews::Folder f;
            folder(xml, &f);
            res->folders << f;
        } else if (xml.name().compare(QStringLiteral("CalendarFolder"), Qt::CaseInsensitive) == 0) {
            ews::Folder f;
            calendarFolder(xml, &f);
            res->folders << f;
        } else {
            xml.skipCurrentElement();
        }
    }
}

void folder(QXmlStreamReader &xml, ews::Folder *res)
{
    Q_ASSERT (xml.name().compare(QStringLiteral("Folder"), Qt::CaseInsensitive) == 0);
    while (xml.readNextStartElement()) {
        if (xml.name().compare(QStringLiteral("FolderId"), Qt::CaseInsensitive) == 0)
            res->folderId = xml.attributes().value("Id").toString();
        else
            xml.skipCurrentElement();
    }
}

void calendarFolder(QXmlStreamReader &xml, ews::Folder *res)
{
    Q_ASSERT (xml.name().compare(QStringLiteral("CalendarFolder"), Qt::CaseInsensitive) == 0);
    while (xml.readNextStartElement()) {
        if (xml.name().compare(QStringLiteral("FolderId"), Qt::CaseInsensitive) == 0)
            res->folderId = xml.attributes().value("Id").toString();
        else
            xml.skipCurrentElement();
    }
}

void rootFolder(QXmlStreamReader &xml, ews::FindItemResponse *res)
{
    Q_ASSERT (xml.name().compare(QStringLiteral("RootFolder"), Qt::CaseInsensitive) == 0);
    while (xml.readNextStartElement()) {
        if (xml.name().compare(QStringLiteral("Items"), Qt::CaseInsensitive) == 0)
            items(xml, res);
        else
            xml.skipCurrentElement();
    }
}

void items(QXmlStreamReader &xml, ews::FindItemResponse *res)
{
    Q_ASSERT (xml.name().compare(QStringLiteral("Items"), Qt::CaseInsensitive) == 0);
    while (xml.readNextStartElement()) {
        if (xml.name().compare(QStringLiteral("CalendarItem"), Qt::CaseInsensitive) == 0)
            calendarItem(xml, res);
        else
            xml.skipCurrentElement();
    }
}

void calendarItem(QXmlStreamReader &xml, ews::FindItemResponse *res)
{
    ews::CalendarItem item;
    Q_ASSERT (xml.name().compare(QStringLiteral("CalendarItem"), Qt::CaseInsensitive) == 0);
    while (xml.readNextStartElement()) {
        if (xml.name().compare(QStringLiteral("Subject"), Qt::CaseInsensitive) == 0)
            subject(xml, &item);
        else if (xml.name().compare(QStringLiteral("Start"), Qt::CaseInsensitive) == 0)
            start(xml, &item);
        else if (xml.name().compare(QStringLiteral("End"), Qt::CaseInsensitive) == 0)
            end(xml, &item);
        else if (xml.name().compare(QStringLiteral("Location"), Qt::CaseInsensitive) == 0)
            location(xml, &item);
        else
            xml.skipCurrentElement();
    }
    res->calendarItems << item;
}

void subject(QXmlStreamReader &xml, ews::CalendarItem *item)
{
    Q_ASSERT (xml.name().compare(QStringLiteral("Subject"), Qt::CaseInsensitive) == 0);
    item->subject = xml.readElementText();
    if (!xml.isEndElement())
        xml.skipCurrentElement();
}

void start(QXmlStreamReader &xml, ews::CalendarItem *item)
{
    Q_ASSERT (xml.name().compare(QStringLiteral("Start"), Qt::CaseInsensitive) == 0);
    item->start = QDateTime::fromString(xml.readElementText(), Qt::ISODate);
    if (!xml.isEndElement())
        xml.skipCurrentElement();
}

void end(QXmlStreamReader &xml, ews::CalendarItem *item)
{
    Q_ASSERT (xml.name().compare(QStringLiteral("End"), Qt::CaseInsensitive) == 0);
    item->end = QDateTime::fromString(xml.readElementText(), Qt::ISODate);
    if (!xml.isEndElement())
        xml.skipCurrentElement();
}

void location(QXmlStreamReader &xml, ews::CalendarItem *item)
{
    Q_ASSERT (xml.name().compare(QStringLiteral("Location"), Qt::CaseInsensitive) == 0);
    item->location = xml.readElementText();
    if (!xml.isEndElement())
        xml.skipCurrentElement();
}

} // namespace parser
