#ifndef CHATWINDOW_H
#define CHATWINDOW_H

#include <QLineEdit>
#include <QListView>
#include <QMainWindow>
#include <QPushButton>
#include <QSplitter>
#include <QTextEdit>
#include <QVBoxLayout>
#include "chatwidget.h"
#include "client.h"
#include "userlistwidget.h"

class ChatWindow : public QMainWindow {
  Q_OBJECT

 public:
  ChatWindow(const QString& ip, const QString& username, const QString& password, QWidget *parent = nullptr);
  ~ChatWindow();

 private slots:
  void onSendButtonClicked();

 private:

  std::shared_ptr<ChatClient> client;
  std::mutex mutex;
  ChatWidget* chat;
  QLineEdit* inputField;
  UserListWidget* userList;
  QPushButton* sendButton;
};

#endif // CHATWINDOW_H
