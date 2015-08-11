#include <QString>
#include <QtTest>

#include "parser.h"

class TestParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void getFolderResponse();
    void getUserAvailabilityResponse();
};

void TestParser::getFolderResponse()
{
    QFile f (":/GetFolder-response.xml");
    f.open(QIODevice::ReadOnly);
    ews::GetFolderResponse resp (f.readAll());
    QCOMPARE (resp.response, ews::NoError);
    QCOMPARE (resp.folders.count(), 1);
    QCOMPARE (resp.folders.first().folderId, QStringLiteral("AQApA="));
}

void TestParser::getUserAvailabilityResponse()
{
    QFile f (":/GetUserAvailability-response.xml");
    f.open(QIODevice::ReadOnly);
    ews::GetUserAvailabilityResponse resp (f.readAll());
    QCOMPARE (resp.response, ews::NoError);
    QCOMPARE (resp.events.count(), 4);
    QCOMPARE (resp.events.first().startTime, QDateTime::fromString(QStringLiteral("2015-08-10T11:30:00"), Qt::ISODate));
    QCOMPARE (resp.events.first().endTime, QDateTime::fromString(QStringLiteral("2015-08-10T12:00:00"), Qt::ISODate));
}

QTEST_APPLESS_MAIN (TestParser)

#include "tst_parser.moc"
