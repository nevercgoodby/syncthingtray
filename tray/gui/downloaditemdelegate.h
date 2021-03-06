#ifndef DOWNLOADITEMDELEGATE_H
#define DOWNLOADITEMDELEGATE_H

#include <QPixmap>
#include <QStyledItemDelegate>

namespace QtGui {

class DownloadItemDelegate : public QStyledItemDelegate {
    Q_OBJECT
public:
    DownloadItemDelegate(QObject *parent);

    void paint(QPainter *, const QStyleOptionViewItem &, const QModelIndex &) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    const QPixmap m_folderIcon;
};
} // namespace QtGui

#endif // DOWNLOADITEMDELEGATE_H
