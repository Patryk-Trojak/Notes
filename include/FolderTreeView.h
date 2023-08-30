#ifndef FOLDERTREEVIEW_H
#define FOLDERTREEVIEW_H
#include "FolderTreeDelegate.h"
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QProxyStyle>
#include <QTreeView>

class FolderTreeViewProxyStyle : public QProxyStyle
{
  public:
    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter,
                       const QWidget *widget) const
    {
        if (element == QStyle::PE_IndicatorItemViewItemDrop)
        {
            // We handle drawing indicator in delegate, bacause we only want to change background color of item.
            // Here we cannot do that since it will be displayed on top of item and therefore on top of icon
            // and text which changes their color. We want to avoid it. We can't also set showDropIndicator to false,
            // because it disables drop on valid items.
        }
    }
};

class FolderTreeView : public QTreeView
{
    Q_OBJECT

  public:
    FolderTreeView(QWidget *parent = nullptr);
    void setModel(QAbstractItemModel *model);
    int getCurrentFolderSelectedId() const;

  signals:
    void newFolderSelected(int selectedFolderId);

  protected:
    void dropEvent(QDropEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);

  protected slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

  private:
    FolderTreeDelegate delegate;
    QLabel *dropTooltip;
    void updateDropTooltip(const QDragMoveEvent *event);
    FolderTreeDelegate::DropIndicatorPosition convertDropIndicatorPosition(QAbstractItemView::DropIndicatorPosition);

  private slots:
    void onCustomContextMenuRequested(const QPoint &pos);
    void deleteFolder(const QModelIndex &index);
};

#endif // FOLDERTREEVIEW_H
