#include "./syncthingnotifier.h"

namespace Data {

/*!
 * \struct SyncthingConnection
 * \brief The SyncthingConnection class emits high-level notification for a given SyncthingConnection.
 *
 * In contrast to the signals provided by the SyncthingConnection class, these signals take further apply
 * further logic and take additional information into account (previous status, service status if known, ...).
 */

void SyncthingNotifier::handleStatusChangedEvent(SyncthingStatus newStatus)
{
    m_currentStatus = newStatus;
}

} // namespace Data
