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
    Q_ENUMS (Status)
    Q_PROPERTY (int count READ count NOTIFY countChanged)
    Q_PROPERTY (QVariantMap query READ query WRITE setQuery NOTIFY queryChanged)
    Q_PROPERTY (int status READ status NOTIFY statusChanged)

public:
    enum Roles {
        BusyTypeRole = Qt::UserRole,
        StartTimeRole,
        EndTimeRole
    };
    enum Status { Null, Ready, Loading, Error };

    UserAvailability(QObject *parent = nullptr);

    int count() const { return m_data.count(); }
    QVariantMap query() const;
    void setQuery(const QVariantMap &query);
    Status status() const { return m_status; }

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int,QByteArray> roleNames() const override;

    Q_INVOKABLE QVariantMap get(int index) const;

signals:
    void queryChanged(const QVariantMap &query) const;
    void countChanged(int count) const;
    void statusChanged(Status) const;

private slots:
    void onFinished();
    void onError(QNetworkReply::NetworkError error);
    void reload();
    void updateModel(const QList<ews::CalendarEvent> &data);

private:
    QString m_mailbox;
    QDateTime m_startDate;
    QDateTime m_endDate;
    Status m_status = Null;
    QList<ews::CalendarEvent> m_data;
};

} // namespace quickews

#endif // USERAVAILABILITY_H
