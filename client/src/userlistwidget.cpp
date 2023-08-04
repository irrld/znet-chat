#include "userlistwidget.h"

UserListWidget::UserListWidget(QWidget *parent) : QWidget(parent) {
  userList = new QListWidget(this);
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->addWidget(userList);
  layout->setContentsMargins(0, 0, 0, 0);
}

void UserListWidget::addUser(int user_id, const QString& username) {
  users[user_id] = username;
  userList->addItem(username);
}

void UserListWidget::removeUser(int user_id) {
  QString username = users[user_id];
  QList<QListWidgetItem*> items = userList->findItems(username, Qt::MatchExactly);
  for (QListWidgetItem* item : items) {
    int row = userList->row(item);
    userList->takeItem(row);
    delete item;
  }
}
