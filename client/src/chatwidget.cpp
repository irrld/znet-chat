#include "chatwidget.h"
#include "wrappedlistitemdelegate.h"

ChatWidget::ChatWidget(QWidget *parent) : QWidget(parent) {
  messageList = new QListWidget(this);
  messageList->setItemDelegate(new WrappedListItemDelegate(messageList)); // Set the custom delegate
  auto* layout = new QVBoxLayout(this);
  layout->addWidget(messageList);
  layout->setContentsMargins(0, 0, 0, 0);
}

void ChatWidget::addMessage(const QString& username, const QString& message) {
  messageList->addItem(username + ": " + message);
}

void ChatWidget::addSystem(const QString& message) {
  auto* item = new QListWidgetItem(message);
  item->setForeground(Qt::red);
  messageList->addItem(item);
}
