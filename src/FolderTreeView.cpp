#include "FolderTreeView.h"

#include "FolderData.h"
#include <QMenu>
#include <QModelIndex>

FolderTreeView::FolderTreeView(QWidget *parent) : QTreeView(parent)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(this, &FolderTreeView::customContextMenuRequested, this,
                     &FolderTreeView::onCustomContextMenuRequested);
}

void FolderTreeView::onCustomContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = QTreeView::indexAt(pos);
    QMenu *menu = new QMenu(this);
    QAction *createSubfolder = new QAction(index.isValid() ? "Create subfolder" : "Create new folder");
    QObject::connect(createSubfolder, &QAction::triggered, this,
                     [this, index]() { this->model()->insertRow(0, index); });
    menu->addAction(createSubfolder);

    if (index.isValid())
    {
        QAction *deleteFolder = new QAction("Delete folder");
        QObject::connect(deleteFolder, &QAction::triggered, this,
                         [this, index]() { this->model()->removeRow(index.row(), index.parent()); });
        menu->addAction(deleteFolder);

        QAction *renameFolder = new QAction("Rename folder");
        QObject::connect(renameFolder, &QAction::triggered, this, [this, index]() { this->edit(index); });
        menu->addAction(renameFolder);
    }

    menu->exec(mapToGlobal(pos));
}

void FolderTreeView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QTreeView::selectionChanged(selected, deselected);
    auto selectedIndexes = selected.indexes();
    if (selectedIndexes.size() > 0)
    {
        FolderData *selectedFolder = static_cast<FolderData *>(selectedIndexes[0].internalPointer());
        emit newFolderSelected(selectedFolder->getId());
    }
}
