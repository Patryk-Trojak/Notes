#ifndef FOLDERTREEVIEW_H
#define FOLDERTREEVIEW_H
#include <QTreeView>

class FolderTreeView : public QTreeView
{
    Q_OBJECT

  public:
    FolderTreeView(QWidget *parent = nullptr);
    void setModel(QAbstractItemModel *model);

  signals:
    void newFolderSelected(int selectedFolderId);

  private slots:
    void onCustomContextMenuRequested(const QPoint &pos);
    void deleteFolder(const QModelIndex &index);

  protected slots:
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
};

#endif // FOLDERTREEVIEW_H
