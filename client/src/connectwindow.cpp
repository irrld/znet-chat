#include "connectwindow.h"
#include <QVBoxLayout>

ConnectWindow::ConnectWindow(QWidget *parent) : QDialog(parent) {
  setWindowTitle("Connect to Chat");
  resize(300, 200);

  auto* layout = new QVBoxLayout(this);

  ipField = new QLineEdit(this);
  ipField->setPlaceholderText("Enter IP Address");
  layout->addWidget(ipField);

  usernameField = new QLineEdit(this);
  usernameField->setPlaceholderText("Enter Username");
  layout->addWidget(usernameField);

  passwordField = new QLineEdit(this);
  passwordField->setPlaceholderText("Enter Password");
  passwordField->setEchoMode(QLineEdit::Password);
  layout->addWidget(passwordField);

  connectButton = new QPushButton("Connect", this);
  layout->addWidget(connectButton);

  connect(connectButton, &QPushButton::clicked, this, &ConnectWindow::onConnectButtonClicked);
}

void ConnectWindow::onConnectButtonClicked() {
  QString ip = ipField->text();
  QString username = usernameField->text();
  QString password = passwordField->text();

  if (!ip.isEmpty() && !username.isEmpty() && !password.isEmpty()) {
    emit connectSuccessful(ip, username, password);
    accept(); // Close the Connect window
  }
}
