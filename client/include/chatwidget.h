#ifndef CHATWIDGET_H
#define CHATWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QStringList>
#include <QVBoxLayout>

class ChatWidget : public QWidget {
  Q_OBJECT

 public:
  ChatWidget(QWidget *parent = nullptr);

  void addMessage(const QString& username, const QString& message);
  void addSystem(const QString& message);

 private:
  QListWidget* messageList;
};

#endif // CHATWIDGET_H
