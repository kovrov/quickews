#include <QtCore/QSettings>
#include <QtCore/QXmlStreamWriter>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

#include "networkaccessmanager.h"
#include "parser.h"
#include "folder.h"

namespace {

QByteArray GetFolder(const QString &distinguishedFolderId, const QString &mailbox)
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
            stream.writeStartElement("m:GetFolder");
            {
                stream.writeStartElement("m:FolderShape");
                {
                    stream.writeTextElement("t:BaseShape", "IdOnly");
                }
                stream.writeEndElement(); // m:FolderShape
                stream.writeStartElement("m:FolderIds");
                {
                    stream.writeStartElement("t:DistinguishedFolderId");
                    stream.writeAttribute("Id", distinguishedFolderId);
                    {
                        stream.writeStartElement("t:Mailbox");
                        {
                            stream.writeTextElement("t:EmailAddress", mailbox);
                            stream.writeTextElement("t:RoutingType", "SMTP");
                        }
                        stream.writeEndElement(); // t:Mailbox
                    }
                    stream.writeEndElement(); // t:DistinguishedFolderId
                }
                stream.writeEndElement(); // m:FolderIds
            }
            stream.writeEndElement(); // m:GetFolder
        }
        stream.writeEndElement(); // soap:Body
    }
    stream.writeEndElement(); // soap:Envelope

    stream.writeEndDocument();
    return data;
}

}

namespace quickews {


Folder::Folder(QObject *parent) :
    QObject (parent)
{
}

Folder::Folder(const QString &distinguishedFolderId, const QString &mailbox, QObject *parent) :
    QObject (parent),
    m_distinguishedFolderId (distinguishedFolderId),
    m_mailbox (mailbox)
{
    reload();
}

void Folder::reload()
{
    QSettings settings;
    const auto &req = makeNetworkRequest(settings.value("ews/url").toString());
    const auto &data = GetFolder(m_distinguishedFolderId, m_mailbox);
    QNetworkReply *reply = manager()->post(req, data);
    void (QNetworkReply::*QNetworkReply_error)(QNetworkReply::NetworkError) = &QNetworkReply::error;
    QObject::connect(reply, QNetworkReply_error, this, &Folder::onError);
    QObject::connect(reply, &QNetworkReply::finished, this, &Folder::onFinished);
}

void Folder::onFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
    Q_ASSERT (reply);

    ews::GetFolderResponse resp (reply->readAll());
    Q_ASSERT (resp.response == ews::NoError);
    if (!resp.folders.isEmpty()) {
        m_id = resp.folders.first().folderId;
        emit folderIdChanged(m_id);
    }
}

void Folder::onError(QNetworkReply::NetworkError error)
{
    qWarning("error %d", error);
}

} // namespace quickews
