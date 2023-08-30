#include "FolderTreeDelegate.h"
#include <QFontMetrics>
#include <QLineEdit>
#include <QPainter>
#include <QPainterPath>

FolderTreeDelegate::FolderTreeDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

void FolderTreeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    const FolderTreeItem *item = static_cast<const FolderTreeItem *>(index.constInternalPointer());
    paintBackground(painter, option, item);
    paintBranchArrow(painter, option, item);
    paintIcon(painter, option, item);
    paintName(painter, option, item);
    paintNotesInsideCount(painter, option, item);

    painter->restore();
}

QSize FolderTreeDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(50, 50);
}

void FolderTreeDelegate::paintBackground(QPainter *painter, const QStyleOptionViewItem &option,
                                         const FolderTreeItem *item) const
{
    if (item == dropIndex.constInternalPointer())
    {
        QPainterPath path;
        path.addRoundedRect(option.rect.adjusted(-15, 1, -1, 0), 20, 20);
        painter->fillPath(path, QColor(218, 232, 240));
        QPen p(QColor(4, 100, 150));
        p.setWidth(2);
        painter->save();
        painter->setPen(p);
        painter->drawPath(path);
        painter->restore();
    }
    else if ((option.state & QStyle::State_Selected))
    {
        QPainterPath path;
        path.addRoundedRect(option.rect.adjusted(-15, 0, 0, 0), 20, 20);
        painter->fillPath(path, QColor(189, 224, 254));
    }
}

void FolderTreeDelegate::paintIcon(QPainter *painter, const QStyleOptionViewItem &option,
                                   const FolderTreeItem *item) const
{
    QSize iconSize = QSize(20, 20);
    QRect iconRect = QRect(option.rect.left(), option.rect.top() + (option.rect.height() - iconSize.height()) / 2,
                           iconSize.width(), iconSize.height());

    QIcon icon;
    if (item->getType() == FolderTreeItem::Type::TrashFolder)
    {
        icon.addFile(QString(":/images/delete.png"));
    }
    else
    {
        if (item->data.getNotesInsideCount() == 0)
            icon.addFile(QString(":/images/emptyFolder.png"));
        else
            icon.addFile(QString(":/images/folderWithNotes.png"));
    }
    painter->drawPixmap(iconRect, icon.pixmap(iconSize));
}

void FolderTreeDelegate::paintName(QPainter *painter, const QStyleOptionViewItem &option,
                                   const FolderTreeItem *item) const
{
    QRect textRect = option.rect.adjusted(31, 0, -28, 0);
    QFont font("Verdana", 11);
    painter->setFont(font);
    QString elidedName = painter->fontMetrics().elidedText(item->data.getName(), Qt::ElideRight, textRect.width());
    painter->drawText(textRect, Qt::AlignVCenter, elidedName);
}

void FolderTreeDelegate::paintNotesInsideCount(QPainter *painter, const QStyleOptionViewItem &option,
                                               const FolderTreeItem *item) const
{
    QRect notesInsideCountRect =
        QRect(option.rect.left() + option.rect.width() - 28, option.rect.top(), 30, option.rect.height());
    painter->setPen(QColor(26, 26, 26, 127));
    painter->drawText(notesInsideCountRect, Qt::AlignCenter, QString::number(item->data.getNotesInsideCount()));
}

void FolderTreeDelegate::paintBranchArrow(QPainter *painter, const QStyleOptionViewItem &option,
                                          const FolderTreeItem *item) const
{
    if (item->getChildren().empty())
        return;

    QSize iconSize = QSize(8, 8);
    QRect iconRect = QRect(option.rect.left() - 12, option.rect.top() + (option.rect.height() - iconSize.height()) / 2,
                           iconSize.width(), iconSize.height());

    QIcon icon(":/images/branchArrow.png");
    if ((option.state & QStyle::State_Open))
    {
        QTransform tr;
        tr.rotate(90);
        auto pixmap = icon.pixmap(iconSize).transformed(tr);
        icon.addPixmap(pixmap);
    }

    painter->drawPixmap(iconRect, icon.pixmap(iconSize));
}

void FolderTreeDelegate::setDropIndex(const QModelIndex &newDropIndex)
{
    dropIndex = newDropIndex;
}

void FolderTreeDelegate::setIndicator(DropIndicatorPosition newIndicator)
{
    dropIndicatorindicator = newIndicator;
}

QWidget *FolderTreeDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option,
                                          const QModelIndex &index) const
{
    QLineEdit *lineEdit = new QLineEdit(parent);
    return lineEdit;
}

void FolderTreeDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option,
                                              const QModelIndex &index) const
{
    editor->setGeometry(option.rect.adjusted(28, 0, -28, 0));
}
