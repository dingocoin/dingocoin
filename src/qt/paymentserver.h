// Copyright (c) 2011-2016 The Bitcoin Core developers
// Copyright (c) 2023 The Dingocoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_PAYMENTSERVER_H
#define BITCOIN_QT_PAYMENTSERVER_H

// This class handles payment requests from clicking on
// bitcoin: URIs
//
// This is somewhat tricky, because we have to deal with
// the situation where the user clicks on a link during
// startup/initialization, when the splash-screen is up
// but the main window (and the Send Coins tab) is not.
//
// So, the strategy is:
//
// Create the server, and register the event handler,
// when the application is created. Save any URIs
// received at or during startup in a list.
//
// When startup is finished and the main window is
// shown, a signal is sent to slot uiReady(), which
// emits a receivedURI() signal for any payment
// requests that happened during startup.
//
// After startup, receivedURI() happens as usual.
//
// This class has one more feature: a static
// method that finds URIs passed in the command line
// and, if a server is running in another process,
// sends them to the server.
//

#include "paymentrequestplus.h"
#include "walletmodel.h"

#include <QObject>
#include <QString>

class OptionsModel;

class CWallet;

QT_BEGIN_NAMESPACE
class QApplication;
class QByteArray;
class QLocalServer;
class QNetworkAccessManager;
class QNetworkReply;
class QSslError;
class QUrl;
QT_END_NAMESPACE

// BIP70 max payment request size in bytes (DoS protection)
static const qint64 BIP70_MAX_PAYMENTREQUEST_SIZE = 50000;

class PaymentServer : public QObject
{
    Q_OBJECT

public:
    // Parse URIs on command line
    // Returns false on error
    static void ipcParseCommandLine(int argc, char *argv[]);

    // Returns true if there were URIs on the command line
    // which were successfully sent to an already-running
    // process.
    // Note: if a payment request is given, SelectParams(MAIN/TESTNET)
    // will be called so we startup in the right mode.
    static bool ipcSendCommandLine();

    // parent should be QApplication object
    PaymentServer(QObject* parent, bool startLocalServer = true);
    PaymentServer(QObject* parent, bool startLocalServer = true, bool enableBip70Flag = false);
    PaymentServer(QObject* parent, QString ipcServerName, bool startLocalServer = true);
    PaymentServer(QObject* parent, QString ipcServerName, bool startLocalServer = true, bool enableBip70Flag = false);
    ~PaymentServer();

    // Load root certificate authorities. Pass NULL (default)
    // to read from the file specified in the -rootcertificates setting,
    // or, if that's not set, to use the system default root certificates.
    // If you pass in a store, you should not X509_STORE_free it: it will be
    // freed either at exit or when another set of CAs are loaded.
    static void LoadRootCAs(X509_STORE* store = NULL);

    // Return certificate store
    static X509_STORE* getCertStore();

    // OptionsModel is used for getting proxy settings and display unit
    void setOptionsModel(OptionsModel *optionsModel);

    // Verify that the payment request network matches the client network
    static bool verifyNetwork(const payments::PaymentDetails& requestDetails);
    // Verify if the payment request is expired
    static bool verifyExpired(const payments::PaymentDetails& requestDetails);
    // Verify the payment request size is valid as per BIP70
    static bool verifySize(qint64 requestSize);
    // Verify the payment request amount is valid
    static bool verifyAmount(const CAmount& requestAmount);

Q_SIGNALS:
    // Fired when a valid payment request is received
    void receivedPaymentRequest(SendCoinsRecipient);

    // Fired when a valid PaymentACK is received
    void receivedPaymentACK(const QString &paymentACKMsg);

    // Fired when a message should be reported to the user
    void message(const QString &title, const QString &message, unsigned int style);

public Q_SLOTS:
    // Signal this when the main window's UI is ready
    // to display payment requests to the user
    void uiReady();

    // Submit Payment message to a merchant, get back PaymentACK:
    void fetchPaymentACK(CWallet* wallet, SendCoinsRecipient recipient, QByteArray transaction);

    // Handle an incoming URI, URI with local file scheme or file
    void handleURIOrFile(const QString& s);

private Q_SLOTS:
    void handleURIConnection();
    void netRequestFinished(QNetworkReply*);
    void reportSslErrors(QNetworkReply*, const QList<QSslError> &);
    void handlePaymentACK(const QString& paymentACKMsg);

protected:
    // Constructor registers this on the parent QApplication to
    // receive QEvent::FileOpen and QEvent:Drop events
    bool eventFilter(QObject *object, QEvent *event);

private:
    static bool readPaymentRequestFromFile(const QString& filename, PaymentRequestPlus& request);
    bool processPaymentRequest(const PaymentRequestPlus& request, SendCoinsRecipient& recipient);
    void fetchRequest(const QUrl& url);

    // Setup networking
    void initNetManager();

    bool saveURIs;                      // true during startup

    void initializeServer(QObject* parent, QString ipcServerName, bool startLocalServer, bool enableBip70Flag);
    bool enableBip70;                   // false by default
    QLocalServer* uriServer;

    QNetworkAccessManager* netManager;  // Used to fetch payment requests

    OptionsModel *optionsModel;
};

#endif // BITCOIN_QT_PAYMENTSERVER_H
