#ifndef FOLDERTREEVIEW_H
#define FOLDERTREEVIEW_H
#include <QTreeView>

class FolderTreeView : public QTreeView
{
    Q_OBJECT

  public:
    FolderTreeView(QWidget *parent = nullptr);

  private slots:
    void onCustomContextMenuRequested(const QPoint &pos);
};

#endif // FOLDERTREEVIEW_H
