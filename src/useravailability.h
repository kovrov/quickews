#ifndef USERAVAILABILITY_H
#define USERAVAILABILITY_H

#include <QtCore/QAbstractListModel>
#include <QtCore/QDateTime>
#include <QtCore/QString>
#include <QtNetwork/QNetworkReply>
#include "parser.h"

namespace quickews {

class UserAvailability : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY (QVariantMap query READ query WRITE setQuery NOTIFY queryChanged)
    enum Roles {
        BusyTypeRole = Qt::UserRole,
        StartTimeRole,
        EndTimeRole
    };

public:
    UserAvailability(QObject *parent = nullptr);

    QVariantMap query() const;
    void setQuery(const QVariantMap &query);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int,QByteArray> roleNames() const override;

signals:
    void queryChanged(const QVariantMap &query) const;

private slots:
    void onFinished();
    void onError(QNetworkReply::NetworkError error);
    void reload();
    void updateModel(const QList<ews::CalendarEvent> &data);

private:
    QString m_mailbox;
    QDateTime m_startDate;
    QDateTime m_endDate;
    QList<ews::CalendarEvent> m_data;
};

} // namespace quickews

#endif // USERAVAILABILITY_H
