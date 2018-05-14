#ifndef GIRACOIN_QT_LOGINDIALOG_H
#define GIRACOIN_QT_LOGINDIALOG_H

#include <QDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QString>
#include <QUrl>
#include <QByteArray>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QSslConfiguration>
#include <QSsl>

namespace Ui {
    class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget* parent = 0);
    ~LoginDialog();

private Q_SLOTS:
    void OnQuit();
    void OnLogin();
    void OnResponse(QNetworkReply* reply);
    void onNetError(QNetworkReply::NetworkError neterror);

private:
    void reject();
    void login(QString username, QString password);

    QLabel* userLabel;
    QLabel* passLabel;
    QLineEdit* userLineEdit;
    QLineEdit* passLineEdit;
    QPushButton* loginButton;
    QPushButton* quitButton;
};

#endif // GIRACOIN_QT_LOGINDIALOG_H