#ifndef TRAY_ICON_H
#define TRAY_ICON_H

#include "./traymenu.h"

#include "../../widgets/misc/dbusstatusnotifier.h"

#include <c++utilities/chrono/datetime.h>

#include <QIcon>
#include <QSystemTrayIcon>

QT_FORWARD_DECLARE_CLASS(QPixmap)
QT_FORWARD_DECLARE_CLASS(QNetworkRequest)

namespace Data {
enum class SyncthingStatus;
enum class SyncthingErrorCategory;
} // namespace Data

namespace QtGui {

enum class TrayIconMessageClickedAction { None, DismissNotification, ShowInternalErrors };

class TrayIcon : public QSystemTrayIcon {
    Q_OBJECT

public:
    TrayIcon(const QString &connectionConfig = QString(), QObject *parent = nullptr);
    TrayMenu &trayMenu();

public slots:
    void showInternalError(
        const QString &errorMsg, Data::SyncthingErrorCategory category, int networkError, const QNetworkRequest &request, const QByteArray &response);
    void showSyncthingNotification(ChronoUtilities::DateTime when, const QString &message);
    void showStatusNotification(Data::SyncthingStatus status);
    void showInternalErrorsDialog();
    void updateStatusIconAndText();

private slots:
    void handleActivated(QSystemTrayIcon::ActivationReason reason);
    void handleMessageClicked();
    void handleConnectionStatusChanged(Data::SyncthingStatus status);
    void handleErrorsCleared();

private:
    bool m_initialized;
    TrayMenu m_trayMenu;
    QMenu m_contextMenu;
    QAction *m_errorsAction;
    Data::SyncthingStatus m_status;
#ifdef QT_UTILITIES_SUPPORT_DBUS_NOTIFICATIONS
    DBusStatusNotifier m_dbusNotifier;
#endif
    TrayIconMessageClickedAction m_messageClickedAction;
};

inline TrayMenu &TrayIcon::trayMenu()
{
    return m_trayMenu;
}
} // namespace QtGui

#endif // TRAY_ICON_H
