#ifndef DATA_SYNCTHINGNOTIFIER_H
#define DATA_SYNCTHINGNOTIFIER_H

#include "./syncthingconnection.h"

//#include "./global.h"

//#include <QObject>

namespace Data {

//enum class SyncthingStatus;
//class SyncthingConnection;

class LIB_SYNCTHING_CONNECTOR_EXPORT SyncthingNotifier : public QObject {
    Q_OBJECT

public:
    SyncthingNotifier(const SyncthingConnection &connection, QObject *parent = nullptr);

Q_SIGNALS:
    void disconnected();
    void syncComplete(const std::vector<SyncthingDir *> &dirs);

private Q_SLOTS:
    void handleStatusChangedEvent(SyncthingStatus newStatus);

private:
    const SyncthingConnection &m_connection;
    SyncthingStatus m_currentStatus;
};

inline SyncthingNotifier::SyncthingNotifier(const SyncthingConnection &connection, QObject *parent)
    : QObject(parent)
    , m_connection(connection)
    , m_currentStatus(SyncthingStatus::Disconnected)
{
}

} // namespace Data

#endif // DATA_SYNCTHINGNOTIFIER_H
