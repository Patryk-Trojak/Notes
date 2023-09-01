#ifndef FOLDERTREEVIEW_H
#define FOLDERTREEVIEW_H
#include "FolderTreeDelegate.h"
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QProxyStyle>
#include <QTreeView>

class FolderTreeViewProxyStyle : public QProxyStyle
{
  public:
    void drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter,
                       const QWidget *widget) const;

    bool drawIndicator{false};
    bool isIndicatorAboveItem{false};
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
    void startDrag(Qt::DropActions supportedActions);

  protected slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

  private:
    FolderTreeDelegate delegate;
    FolderTreeViewProxyStyle *myStyle;
    QLabel *dropTooltip;
    QPoint dropTooltipOffset;
    void updateDropTooltip(const QDragMoveEvent *event);
    void updateTextOfDropTooltip(const QModelIndex &dropIndex, const QMimeData &mimeData);
    void updateSizeOfDropTooltip();
    void updateTextFormatingOfDropTooltip(const QModelIndex &dropIndex);

  private slots:
    void onCustomContextMenuRequested(const QPoint &pos);
    void deleteFolder(const QModelIndex &index);
    QPixmap renderDragPixmap(const QModelIndexList &indexes, QPoint *hotSpot);
    QMessageBox::StandardButton makeSureIfUserWantToDeleteFolder();
};

#endif // FOLDERTREEVIEW_H
