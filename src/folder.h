#ifndef EWS_FOLDER_H
#define EWS_FOLDER_H

#include <QtCore/QObject>
#include <QtNetwork/QNetworkReply>

namespace quickews {

class Folder : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QString folderId READ folderId NOTIFY folderIdChanged)

public:
    explicit Folder(QObject *parent = nullptr);
    explicit Folder(const QString &distinguishedFolderId, const QString &mailbox, QObject *parent = nullptr);
    QString folderId() { return m_id; }

signals:
    void folderIdChanged(const QString &id);

private slots:
    void onFinished();
    void onError(QNetworkReply::NetworkError error);

private:
    void reload();

    QString m_id;
    QString m_distinguishedFolderId;
    QString m_mailbox;
};

} // namespace quickews

#endif // EWS_FOLDER_H
