#include <QtCore/QSettings>
#include <QtCore/QXmlStreamWriter>

#include "networkaccessmanager.h"
#include "parser.h"

#include "useravailability.h"

namespace {

QByteArray GetUserAvailabilityRequest(const QString &mailbox, const QDateTime &start, const QDateTime &end)
{
    QByteArray data;
    QXmlStreamWriter stream(&data);
    stream.writeStartDocument();

    stream.writeStartElement("soap:Envelope");
    stream.writeNamespace("http://schemas.xmlsoap.org/soap/envelope/", "soap");
    stream.writeNamespace("http://schemas.microsoft.com/exchange/services/2006/messages", "m");
    stream.writeNamespace("http://schemas.microsoft.com/exchange/services/2006/types", "t");
    {
        stream.writeStartElement("soap:Body");
        {
            stream.writeStartElement("m:GetUserAvailabilityRequest");
            {
                stream.writeStartElement("t:TimeZone");
                {
                    stream.writeTextElement("t:Bias", "480");
                    stream.writeStartElement("t:StandardTime");
                    {
                        stream.writeTextElement("t:Bias", "0");
                        stream.writeTextElement("t:Time", "02:00:00");
                        stream.writeTextElement("t:DayOrder", "5");
                        stream.writeTextElement("t:Month", "10");
                        stream.writeTextElement("t:DayOfWeek", "Sunday");
                    }
                    stream.writeEndElement(); // t:StandardTime

                    stream.writeStartElement("t:DaylightTime");
                    {
                        stream.writeTextElement("t:Bias", "-60");
                        stream.writeTextElement("t:Time", "02:00:00");
                        stream.writeTextElement("t:DayOrder", "1");
                        stream.writeTextElement("t:Month", "4");
                        stream.writeTextElement("t:DayOfWeek", "Sunday");
                    }
                    stream.writeEndElement(); // t:DaylightTime
                }
                stream.writeEndElement(); // t:TimeZone

                stream.writeStartElement("m:MailboxDataArray");
                {
                    stream.writeStartElement("t:MailboxData");
                    {
                        stream.writeStartElement("t:Email");
                        {
                            stream.writeTextElement("t:Address", mailbox);
                        }
                        stream.writeEndElement(); // t:Email

                        stream.writeTextElement("t:AttendeeType", "Required");
                        stream.writeTextElement("t:ExcludeConflicts", "false");
                    }
                    stream.writeEndElement(); // t:MailboxData
                }
                stream.writeEndElement(); // m:MailboxDataArray

                stream.writeStartElement("t:FreeBusyViewOptions");
                {
                    stream.writeStartElement("t:TimeWindow");
                    {
                        stream.writeTextElement("t:StartTime", start.toString(Qt::ISODate));
                        stream.writeTextElement("t:EndTime", end.toString(Qt::ISODate));
                    }
                    stream.writeEndElement(); // t:TimeWindow

                    stream.writeTextElement("t:MergedFreeBusyIntervalInMinutes", "15");
                    stream.writeTextElement("t:RequestedView", "DetailedMerged");
                }
                stream.writeEndElement(); // t:FreeBusyViewOptions
            }
            stream.writeEndElement(); // m:GetUserAvailabilityRequest
        }
        stream.writeEndElement(); // soap:Body
    }
    stream.writeEndElement(); // soap:Envelope

    stream.writeEndDocument();
    return data;
}

}

namespace quickews {

UserAvailability::UserAvailability(QObject *parent) :
    QAbstractListModel (parent)
{
}

void UserAvailability::setQuery(const QVariantMap &query)
{
    m_mailbox = query.value("mailbox").toString();
    m_endDate = query.value("endDate").toDateTime();
    m_startDate = query.value("startDate").toDateTime();
    reload();
    emit queryChanged(QVariantMap {
                          { "mailbox", m_mailbox },
                          { "startDate", m_startDate },
                          { "endDate", m_endDate }});
}

void UserAvailability::reload()
{
    if (m_mailbox.isEmpty() || !m_startDate.isValid() || !m_endDate.isValid())
        return;

    QSettings settings;
    const QNetworkRequest &req = makeNetworkRequest(settings.value("ews/url").toString());
    const QByteArray &data = GetUserAvailabilityRequest(m_mailbox, m_startDate, m_endDate);
    QNetworkReply *reply = manager()->post(req, data);
    void (QNetworkReply::*QNetworkReply_error)(QNetworkReply::NetworkError) = &QNetworkReply::error;
    QObject::connect(reply, QNetworkReply_error, this, &UserAvailability::onError);
    QObject::connect(reply, &QNetworkReply::finished, this, &UserAvailability::onFinished);
}

void UserAvailability::onFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT (reply);

    ews::GetUserAvailabilityResponse resp (reply->readAll());
    if (resp.response == ews::NoError) {
        updateModel(resp.events);
    } else {
        // TODO: set an error state
    }
}

void UserAvailability::updateModel(const QList<ews::CalendarEvent> &data)
{
    // TODO: calculate diffs
    beginResetModel();
    m_data = data;
    endResetModel();
    emit countChanged(m_data.count());
}

void UserAvailability::onError(QNetworkReply::NetworkError error)
{
    qWarning("error %d", error);
}

int UserAvailability::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED (parent)
    return m_data.count();
}

QVariant UserAvailability::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case BusyTypeRole:
        return QVariant(m_data.at(index.row()).busyType);
    case StartTimeRole:
        return QVariant(m_data.at(index.row()).startTime);
    case EndTimeRole:
        return QVariant(m_data.at(index.row()).endTime);
    default:
        return QVariant();
    }
}

QHash<int,QByteArray> UserAvailability::roleNames() const
{
    static const QHash<int,QByteArray> _roleNames {
        { BusyTypeRole, "busyType" },
        { StartTimeRole, "startTime" },
        { EndTimeRole, "endTime" }
    };
    return _roleNames;
}

QVariantMap UserAvailability::get(int index) const
{
    auto it = m_data.at(index);
    return QVariantMap {
        { "busyType", it.busyType },
        { "startTime", it.startTime },
        { "endTime", it.endTime }
    };
}

} // namespace quickews
