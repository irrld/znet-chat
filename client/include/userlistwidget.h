#ifndef USERLISTWIDGET_H
#define USERLISTWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QStringList>
#include <QVBoxLayout>

class UserListWidget : public QWidget {
  Q_OBJECT

 public:
  UserListWidget(QWidget *parent = nullptr);

  void addUser(int user_id, const QString& username);
  void removeUser(int user_id);

 private:
  QMap<int, QString> users;
  QListWidget *userList;
};

#endif // USERLISTWIDGET_H
