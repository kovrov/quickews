#include <QString>
#include <QtTest>

#include "parser.h"

class TestParser : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void getFolderResponse();
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

QTEST_APPLESS_MAIN (TestParser)

#include "tst_parser.moc"
