#include <QtCore/QSettings>
#include <QtNetwork/QAuthenticator>
#include <QtNetwork/QNetworkReply>

#include "networkaccessmanager.h"

QNetworkAccessManager *manager()
{
    static QNetworkAccessManager * _mgr = nullptr;
    if (nullptr == _mgr) {
        _mgr = new QNetworkAccessManager();
        QObject::connect(_mgr, &QNetworkAccessManager::authenticationRequired,
                         [](QNetworkReply *reply, QAuthenticator *authenticator) {
            Q_UNUSED (reply)
            QSettings settings;
            authenticator->setUser(settings.value("ews/username").toString());
            authenticator->setPassword(settings.value("ews/password").toString());
        });
        QObject::connect(_mgr, &QNetworkAccessManager::sslErrors,
                         [](QNetworkReply *reply, const QList<QSslError> &) {
            reply->ignoreSslErrors();
        });
    }
    return _mgr;
}

QNetworkRequest makeNetworkRequest(const QUrl &url)
{
    QNetworkRequest req(url);
    req.setRawHeader("User-Agent", "QuickEWS/0.1");
    req.setRawHeader("Content-Type", "text/xml");
    req.setRawHeader("Accept", "text/xml; charset=utf-8");
    QSslConfiguration config;
    config.setProtocol(QSsl::AnyProtocol);
    req.setSslConfiguration(config);
    return req;
}
