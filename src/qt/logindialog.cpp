#include "logindialog.h"

LoginDialog::LoginDialog(QWidget* parent)
    : QDialog(parent)
{
    setFixedSize(300, 120);
    setWindowTitle("Girapay Login");
    setModal(true);
    setAttribute(Qt::WA_DeleteOnClose);

    userLabel = new QLabel("Username:");
    passLabel = new QLabel("Password:");
    userLineEdit = new QLineEdit();
    passLineEdit = new QLineEdit();
    passLineEdit->setEchoMode(QLineEdit::Password);
    loginButton = new QPushButton("Login");
    quitButton = new QPushButton("Quit");

    QVBoxLayout* vbox = new QVBoxLayout(this);
    QHBoxLayout* hbox1 = new QHBoxLayout();
    QHBoxLayout* hbox2 = new QHBoxLayout();
    QHBoxLayout* hbox3 = new QHBoxLayout();

    hbox1->addWidget(userLabel, 1);
    hbox1->addWidget(userLineEdit, 2);
    hbox2->addWidget(passLabel, 1);
    hbox2->addWidget(passLineEdit, 2);
    hbox3->addWidget(loginButton, 1, Qt::AlignRight);
    hbox3->addWidget(quitButton, 0, Qt::AlignRight);

    vbox->addSpacing(1);
    vbox->addLayout(hbox1);
    vbox->addLayout(hbox2);
    vbox->addLayout(hbox3);

    connect(quitButton, SIGNAL(clicked()), this, SLOT(OnQuit()));
    connect(loginButton, SIGNAL(clicked()), this, SLOT(OnLogin()));
}

void LoginDialog::reject()
{
    OnQuit();
}

void LoginDialog::OnQuit()
{
    close();
    parentWidget()->close();
}

void LoginDialog::OnLogin()
{
    QString username = userLineEdit->text();
    QString password = passLineEdit->text();

    // Checking if username or password is empty
    if (username.isEmpty() || password.isEmpty())
        QMessageBox::information(this, tr("Error!"), "Username and password are required");
    else
        login(username, password);
}

void LoginDialog::OnResponse(QNetworkReply* reply)
{
    QVariant statusCode = reply->attribute( QNetworkRequest::HttpStatusCodeAttribute );

    if (!statusCode.isValid()) {
        reply->deleteLater();
        QMessageBox::information(this, tr("Error!"), "Service not reachable or unavailable, please try again later.");
        OnQuit();
    }

    int status = statusCode.toInt();

    if (status == 200 || status == 201) {
        reply->deleteLater();
        destroy();
    } else if (status != 401) {
        reply->deleteLater();
        QMessageBox::information(this, tr("Error!"), "Unknown error occurred, please try again later.");
        OnQuit();
    }
}

void LoginDialog::onNetError(QNetworkReply::NetworkError neterror)
{
    if (neterror == QNetworkReply::AuthenticationRequiredError) {
        QMessageBox::information(this, tr("Error!"), "Username and password do not match.");
        this->userLineEdit->setText(QString(""));
        this->passLineEdit->setText(QString(""));
    } else
        QMessageBox::information(this, tr("Error!"), "Service not reachable or unavailable, please try again later.");
}

void LoginDialog::login(QString username, QString password)
{
    QUrl endpoint("https://girapay.com/api/v1/mobile/login");
    QNetworkRequest request(endpoint);

    QString jsonString = "{\"login\":\"" + username + "\",\"password\":\"" + password + "\"}";
    QByteArray jsonBody = jsonString.toUtf8();
    QByteArray postDataSize = QByteArray::number(jsonBody.size());

    request.setHeader(QNetworkRequest::UserAgentHeader, "Desktop Wallet");
    request.setRawHeader("X-Custom-User-Agent", "Desktop Wallet");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::ContentLengthHeader, postDataSize);
    request.setRawHeader("Accept", "application/json");

    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2);
    request.setSslConfiguration(config);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);

    QNetworkReply* reply = manager->post(request, jsonBody);

    connect(reply, SIGNAL(finished(QNetworkReply*)), this, SLOT(OnResponse(QNetworkReply*)));
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(OnResponse(QNetworkReply*)));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), SLOT(onNetError(QNetworkReply::NetworkError)));
}

LoginDialog::~LoginDialog() {}