#include "wrappedlistitemdelegate.h"
#include <QAbstractTextDocumentLayout>
#include <QListWidgetItem>
#include <QPainter>
#include <QTextDocument>

WrappedListItemDelegate::WrappedListItemDelegate(QObject *parent)
    : QStyledItemDelegate(parent) {}

QSize WrappedListItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
  Q_UNUSED(option);
  if (!index.isValid())
    return QSize();

  QTextDocument doc;
  doc.setHtml(index.data(Qt::DisplayRole).toString());
  doc.setTextWidth(option.rect.width());
  int height = qMax(15, static_cast<int>(doc.size().height()));

  return {option.rect.width(), height};
}

void WrappedListItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
  QString text = index.data(Qt::DisplayRole).toString();
  auto* item = static_cast<QListWidgetItem*>(index.internalPointer());

  if (!item) {
    return;
  }

  painter->save();
  painter->translate(option.rect.topLeft());
  QTextDocument doc;
  doc.setTextWidth(option.rect.width());
  if (item->foreground() != Qt::NoBrush) {
    const QColor& color = item->foreground().color();
    doc.setHtml("<p style=\"color: " + color.name() + "\">" + text + "</p>");
  } else {
    doc.setHtml(text);
  }
  doc.drawContents(painter);
  painter->restore();
}
