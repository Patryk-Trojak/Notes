#include "FolderTreeView.h"

#include "FolderData.h"
#include "FolderTreeModel.h"
#include "PersistenceManager.h"
#include <QDragMoveEvent>
#include <QMenu>
#include <QMessageBox>
#include <QModelIndex>

FolderTreeView::FolderTreeView(QWidget *parent) : QTreeView(parent), delegate(this)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(this, &FolderTreeView::customContextMenuRequested, this,
                     &FolderTreeView::onCustomContextMenuRequested);
    setStyleSheet("FolderTreeView{border: none; selection-color: transparent; selection-background-color: transparent;}"
                  "QTreeView::branch { background: transparent; } ");
    setItemDelegate(&delegate);

    int widthOfScrollbar = 5;
    QString style =
        QString(
            "FolderTreeView{border: none; selection-color: transparent; selection-background-color: transparent;}"
            "QTreeView::branch { background: transparent; } "
            "QScrollBar { margin-left: 2px; width: %1px; background: transparent; } "
            "QScrollBar::handle:vertical:hover {background: rgba(40, 40, 40, 0.5);} "
            "QScrollBar::handle:vertical:pressed {background: rgba(40, 40, 40, 0.5);}"
            "QScrollBar::handle:vertical {border-radius: 4px;background: rgba(100, 100, 100, 0.5);min-height: 20px;}"
            "QScrollBar::vertical {border-radius: 6px;width: 10px;color: rgba(255, 255, 255, 0);}"
            "QScrollBar::add-line:vertical {width: 0px; height: 0px;subcontrol-position: bottom;subcontrol-origin: "
            "margin;}"
            "QScrollBar::sub-line:vertical {width: 0px;height: 0px;subcontrol-position: top;subcontrol-origin: "
            "margin;}")
            .arg(widthOfScrollbar);
    setStyleSheet(style);

    setStyle(new FolderTreeViewProxyStyle);
    setDragDropOverwriteMode(true);
    setDropIndicatorShown(true);
    dropTooltip = new QLabel(QWidget::window()); // We may want part of it to be drawn outside of the folder tree view
    dropTooltip->setStyleSheet("background-color: white;"
                               "border-style: solid; border-color: black; border-width: 2px; border-radius: 8px;");
    dropTooltip->setAttribute(Qt::WA_StyledBackground);
    dropTooltip->setVisible(false);
    dropTooltip->setAlignment(Qt::AlignCenter);
}

void FolderTreeView::setModel(QAbstractItemModel *model)
{
    QTreeView::setModel(model);
    if (this->model()->rowCount() > 0)
        setCurrentIndex(this->model()->index(0, 0));
}

int FolderTreeView::getCurrentFolderSelectedId() const
{
    FolderData *selectedFolder = static_cast<FolderData *>(currentIndex().internalPointer());
    if (selectedFolder)
        return selectedFolder->getId();
    else
        return SpecialFolderId::AllNotesFolder;
}

void FolderTreeView::onCustomContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = QTreeView::indexAt(pos);
    if (!(index.flags() & Qt::ItemIsEditable))
        index = QModelIndex(); // Treat uneditable item as when the mouse isn't over any item

    QMenu *menu = new QMenu(this);
    QAction *createSubfolder = new QAction(index.isValid() ? "Create subfolder" : "Create new folder");
    QObject::connect(createSubfolder, &QAction::triggered, this,
                     [this, index]() { this->model()->insertRow(0, index); });
    menu->addAction(createSubfolder);

    if (index.isValid())
    {
        QAction *deleteFolder = new QAction("Delete folder");
        QObject::connect(deleteFolder, &QAction::triggered, this, [this, index]() { this->deleteFolder(index); });
        menu->addAction(deleteFolder);

        QAction *renameFolder = new QAction("Rename folder");
        QObject::connect(renameFolder, &QAction::triggered, this, [this, index]() { this->edit(index); });
        menu->addAction(renameFolder);
    }

    menu->exec(mapToGlobal(pos));
}

void FolderTreeView::deleteFolder(const QModelIndex &index)
{
    auto reply =
        QMessageBox::question(this, "Delete folder?",
                              "Are you sure you want to delete this folder? All notes and subfolders will be removed.");
    if (reply == QMessageBox::No)
        return;

    model()->removeRow(index.row(), index.parent());
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

void FolderTreeView::updateDropTooltip(const QDragMoveEvent *event)
{
    QModelIndex dropIndex = indexAt(event->position().toPoint());
    FolderTreeModel *folderModel = static_cast<FolderTreeModel *>(model());
    QString textOfTooltip;
    if (folderModel->areAllDragingNotesInFolderIndex(dropIndex, event->mimeData()))
        textOfTooltip = "Notes already in this folder";
    else
        textOfTooltip = "Move to: " + dropIndex.data().toString();

    dropTooltip->move(QWidget::window()->mapFromGlobal(this->mapToGlobal(event->position().toPoint())) + QPoint(5, 5));
    QSize sizeOftooltip = dropTooltip->fontMetrics().size(Qt::TextSingleLine, textOfTooltip);
    sizeOftooltip.setWidth(qMin(sizeOftooltip.width(), 300));
    QString elidedText = dropTooltip->fontMetrics().elidedText(textOfTooltip, Qt::ElideRight, sizeOftooltip.width());
    if (!folderModel->areAllDragingNotesInFolderIndex(dropIndex, event->mimeData()))
    {
        elidedText.insert(8, "</font>");
        elidedText.insert(0, "<font color = #1741d5>");
    }
    dropTooltip->setText(elidedText);
    sizeOftooltip = sizeOftooltip.grownBy(QMargins(10, 5, 10, 10));
    dropTooltip->resize(sizeOftooltip);
    dropTooltip->setVisible(true);
}

void FolderTreeView::dropEvent(QDropEvent *event)
{
    QTreeView::dropEvent(event);
    delegate.setDropIndex(QModelIndex());
    dropTooltip->setVisible(false);
}

void FolderTreeView::dragMoveEvent(QDragMoveEvent *event)
{
    QTreeView::dragMoveEvent(event);
    QModelIndex dropIndex = indexAt(event->position().toPoint());
    delegate.setDropIndex(dropIndex);

    if (!dropIndex.isValid())
    {
        dropTooltip->setVisible(false);
        return;
    }
    updateDropTooltip(event);
}

void FolderTreeView::dragLeaveEvent(QDragLeaveEvent *event)
{
    QAbstractItemView::dragLeaveEvent(event);
    delegate.setDropIndex(QModelIndex());
    dropTooltip->setVisible(false);
}

void FolderTreeView::dragEnterEvent(QDragEnterEvent *event)
{
    QTreeView::dragEnterEvent(event);
}
