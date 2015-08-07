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
