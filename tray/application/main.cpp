#include "./singleinstance.h"

#include "../gui/trayicon.h"
#include "../gui/traywidget.h"

#include "../../widgets/settings/settings.h"

#include "../../connector/syncthingprocess.h"
#ifdef LIB_SYNCTHING_CONNECTOR_SUPPORT_SYSTEMD
#include "../../connector/syncthingservice.h"
#endif

#include "resources/config.h"

#include <c++utilities/application/argumentparser.h>
#include <c++utilities/application/commandlineutils.h>
#include <c++utilities/application/failure.h>

#include <qtutilities/resources/importplugin.h>
#include <qtutilities/resources/qtconfigarguments.h>
#include <qtutilities/resources/resources.h>
#include <qtutilities/settingsdialog/qtsettings.h>

#include <QApplication>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QStringBuilder>

#include <iostream>

using namespace std;
using namespace ApplicationUtilities;
using namespace QtGui;
using namespace Data;

ENABLE_QT_RESOURCES_OF_STATIC_DEPENDENCIES

#ifdef LIB_SYNCTHING_CONNECTOR_SUPPORT_SYSTEMD
void handleSystemdServiceError(const QString &context, const QString &name, const QString &message)
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Critical);
    msgBox.setText(QCoreApplication::translate("main", "Unable to ") + context);
    msgBox.setInformativeText(name % QStringLiteral(":\n") % message);
    msgBox.exec();
}
#endif

int initSyncthingTray(bool windowed, bool waitForTray, const char *connectionConfig)
{
    auto &v = Settings::values();
#ifdef LIB_SYNCTHING_CONNECTOR_SUPPORT_SYSTEMD
    SyncthingService &service = syncthingService();
    service.setUnitName(v.systemd.syncthingUnit);
    QObject::connect(&service, &SyncthingService::errorOccurred, &handleSystemdServiceError);
#endif
    const QString connectionConfigQStr = connectionConfig ? QString::fromLocal8Bit(connectionConfig) : QString();
    if (windowed) {
        v.launcher.autostart();
        auto *trayWidget = new TrayWidget(connectionConfigQStr);
        trayWidget->setAttribute(Qt::WA_DeleteOnClose);
        trayWidget->show();
    } else {
#ifndef QT_NO_SYSTEMTRAYICON
        if (QSystemTrayIcon::isSystemTrayAvailable() || waitForTray) {
            v.launcher.autostart();
            auto *trayIcon = new TrayIcon(connectionConfigQStr);
            trayIcon->show();
            if (v.firstLaunch) {
                QMessageBox msgBox;
                msgBox.setIcon(QMessageBox::Information);
                msgBox.setText(
                    QCoreApplication::translate("main", "You must configure how to connect to Syncthing when using Syncthing Tray the first time."));
                msgBox.setInformativeText(QCoreApplication::translate(
                    "main", "Note that the settings dialog allows importing URL, credentials and API-key from the local Syncthing configuration."));
                msgBox.exec();
                trayIcon->trayMenu().widget()->showSettingsDialog();
            }
        } else {
            QMessageBox::critical(nullptr, QApplication::applicationName(),
                QApplication::translate("main",
                    "The system tray is (currently) not available. You could open the tray menu as a regular window using the -w flag, though."));
            return -1;
        }
#else
        QMessageBox::critical(nullptr, QApplication::applicationName(),
            QApplication::translate("main",
                "The Qt libraries have not been built with tray icon support. You could open the tray menu as a regular "
                "window using the -w flag, though."));
        return -2;
#endif
    }
    return 0;
}

void trigger(bool tray, bool webUi)
{
    if (!TrayWidget::instances().empty() && (tray || webUi)) {
        TrayWidget *trayWidget = TrayWidget::instances().front();
        if (webUi) {
            trayWidget->showWebUi();
        }
        if (tray) {
            trayWidget->showAtCursor();
        }
    }
}

int runApplication(int argc, const char *const *argv)
{
    static bool firstRun = true;

    // setup argument parser
    SET_APPLICATION_INFO;
    CMD_UTILS_CONVERT_ARGS_TO_UTF8;
    ArgumentParser parser;
    HelpArgument helpArg(parser);
    NoColorArgument noColor;
    // Qt configuration arguments
    QT_CONFIG_ARGUMENTS qtConfigArgs;
    Argument windowedArg("windowed", 'w', "opens the tray menu as a regular window");
    windowedArg.setCombinable(true);
    Argument showWebUiArg("webui", '\0', "instantly shows the web UI - meant for creating shortcut to web UI");
    showWebUiArg.setCombinable(true);
    Argument triggerArg("trigger", '\0', "instantly shows the left-click tray menu - meant for creating a shortcut");
    triggerArg.setCombinable(true);
    Argument waitForTrayArg("wait", '\0',
        "wait until the system tray becomes available instead of showing an error message if the system tray is not available on start-up");
    waitForTrayArg.setCombinable(true);
    ConfigValueArgument connectionArg("connection", '\0', "specifies the connection configuration to be used", { "config name" });
    Argument &widgetsGuiArg = qtConfigArgs.qtWidgetsGuiArg();
    widgetsGuiArg.addSubArgument(&windowedArg);
    widgetsGuiArg.addSubArgument(&showWebUiArg);
    widgetsGuiArg.addSubArgument(&triggerArg);
    widgetsGuiArg.addSubArgument(&waitForTrayArg);
    widgetsGuiArg.addSubArgument(&connectionArg);

    parser.setMainArguments({ &qtConfigArgs.qtWidgetsGuiArg(), &noColor, &helpArg });
    parser.parseArgsOrExit(argc, argv);
    if (!qtConfigArgs.qtWidgetsGuiArg().isPresent()) {
        return 0;
    }
    if (firstRun) {
        firstRun = false;

        SET_QT_APPLICATION_INFO;
        QApplication application(argc, const_cast<char **>(argv));
        QGuiApplication::setQuitOnLastWindowClosed(false);
        SingleInstance singleInstance(argc, argv);
        networkAccessManager().setParent(&singleInstance);
        QObject::connect(&singleInstance, &SingleInstance::newInstance, &runApplication);

        Settings::restore();
        Settings::values().qt.apply();
        qtConfigArgs.applySettings(true);
        LOAD_QT_TRANSLATIONS;

        int res = initSyncthingTray(windowedArg.isPresent(), waitForTrayArg.isPresent(), connectionArg.firstValue());
        if (!res) {
            trigger(triggerArg.isPresent(), showWebUiArg.isPresent());
            res = application.exec();
        }

        Settings::Launcher::terminate();
        Settings::save();
        return res;
    } else {
        if (!TrayWidget::instances().empty() && (showWebUiArg.isPresent() || triggerArg.isPresent())) {
            // if --webui or --trigger is present don't create a new tray icon, just trigger actions
            trigger(triggerArg.isPresent(), showWebUiArg.isPresent());
        } else {
            const int res = initSyncthingTray(windowedArg.isPresent(), waitForTrayArg.isPresent(), connectionArg.firstValue());
            if (!res) {
                trigger(triggerArg.isPresent(), showWebUiArg.isPresent());
            }
            return res;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    return runApplication(argc, argv);
}
