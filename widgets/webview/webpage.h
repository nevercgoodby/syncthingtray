#ifndef WEBPAGE_H
#define WEBPAGE_H
#ifndef SYNCTHINGWIDGETS_NO_WEBVIEW

#include "./webviewdefs.h"
#include "./webviewincludes.h"

#include "../global.h"

QT_FORWARD_DECLARE_CLASS(QAuthenticator)
QT_FORWARD_DECLARE_CLASS(QNetworkReply)
QT_FORWARD_DECLARE_CLASS(QNetworkRequest)
QT_FORWARD_DECLARE_CLASS(QSslError)

namespace QtGui {

class WebViewDialog;

class SYNCTHINGWIDGETS_EXPORT WebPage : public SYNCTHINGWIDGETS_WEB_PAGE {
    Q_OBJECT
public:
    WebPage(WebViewDialog *dlg = nullptr, SYNCTHINGWIDGETS_WEB_VIEW *view = nullptr);

    static bool isSamePage(const QUrl &url1, const QUrl &url2);

protected:
    SYNCTHINGWIDGETS_WEB_PAGE *createWindow(WebWindowType type);
#ifdef SYNCTHINGWIDGETS_USE_WEBENGINE
    bool certificateError(const QWebEngineCertificateError &certificateError);
    bool acceptNavigationRequest(const QUrl &url, NavigationType type, bool isMainFrame);
    void javaScriptConsoleMessage(
        QWebEnginePage::JavaScriptConsoleMessageLevel level, const QString &message, int lineNumber, const QString &sourceID);
#else
    bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type);
    void javaScriptConsoleMessage(const QString &message, int lineNumber, const QString &sourceID);
#endif

private slots:
    void delegateNewWindowToExternalBrowser(const QUrl &url);
    void supplyCredentials(const QUrl &requestUrl, QAuthenticator *authenticator);
    void supplyCredentials(QNetworkReply *reply, QAuthenticator *authenticator);
    void supplyCredentials(QAuthenticator *authenticator);
#ifdef SYNCTHINGWIDGETS_USE_WEBKIT
    void handleSslErrors(QNetworkReply *, const QList<QSslError> &errors);
#endif
    void injectJavaScripts(bool ok);
    void processJavaScriptConsoleMessage(const QString &message);
    void injectJavaScript(const QString &scriptSource);
    void showFolderPathSelection(const QString &defaultDir);

private:
    static bool handleNavigationRequest(const QUrl &currentUrl, const QUrl &url);

    WebViewDialog *m_dlg;
    SYNCTHINGWIDGETS_WEB_VIEW *m_view;
};
} // namespace QtGui

#endif // SYNCTHINGWIDGETS_NO_WEBVIEW
#endif // WEBPAGE_H
