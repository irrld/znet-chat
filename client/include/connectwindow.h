#ifndef CONNECTWINDOW_H
#define CONNECTWINDOW_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class ConnectWindow : public QDialog {
  Q_OBJECT

 public:
  ConnectWindow(QWidget *parent = nullptr);

 signals:
  void connectSuccessful(const QString &ip, const QString &username, const QString &password);

 private slots:
  void onConnectButtonClicked();

 public:
  QLineEdit* ipField;
  QLineEdit* usernameField;
  QLineEdit* passwordField;
  QPushButton* connectButton;
};

#endif // CONNECTWINDOW_H
