#include <QApplication>
#include "chatwindow.h"
#include "connectwindow.h"

int main(int argc, char** argv) {
  QApplication app{argc, argv};

  ConnectWindow connectWindow{};
  if (connectWindow.exec() == QDialog::Accepted) {
    QString ip = connectWindow.ipField->text();
    QString username = connectWindow.usernameField->text();
    QString password = connectWindow.passwordField->text();
    ChatWindow chatWindow{ip, username, password};
    chatWindow.show();
    return QApplication::exec();
  }

  return 0;
}