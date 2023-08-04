#ifndef WRAPPEDLISTITEMDELEGATE_H
#define WRAPPEDLISTITEMDELEGATE_H

#include <QStyledItemDelegate>

class WrappedListItemDelegate : public QStyledItemDelegate {
 public:
  WrappedListItemDelegate(QObject *parent = nullptr);

  QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
  void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // WRAPPEDLISTITEMDELEGATE_H
