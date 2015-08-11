#ifndef NETWORKACCESSMANAGER_H
#define NETWORKACCESSMANAGER_H

#include <QtNetwork/QNetworkAccessManager>

QNetworkAccessManager *manager();

QNetworkRequest makeNetworkRequest(const QUrl &url);

#endif // NETWORKACCESSMANAGER_H
