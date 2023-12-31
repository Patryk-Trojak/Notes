#include "FolderTreeView.h"
#include "ColorPicker.h"
#include "FolderData.h"
#include "FolderTreeModel.h"
#include "NoteMimeData.h"
#include "SpecialFolderId.h"
#include <QDrag>
#include <QDragMoveEvent>
#include <QMenu>
#include <QModelIndex>
#include <QPainter>
#include <QRegularExpression>
#include <QScrollBar>
#include <QWidgetAction>

void FolderTreeViewProxyStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter,
                                             const QWidget *widget) const
{
    if (element != QStyle::PE_IndicatorItemViewItemDrop)
        QProxyStyle::drawPrimitive(element, option, painter, widget);

    if (!drawIndicator)
        return;
    // Qt doesn't update indicator when you start draging over undroppable item. In this case, we don't want indicator
    // to be drawn

    if (option->rect.height() > 3)
        return;
    // We handle drawing indicator when is over item in delegate, bacause we only want to change background color of
    // item. Here we cannot do that since it will be displayed on top of item and therefore on top of icon and text
    // which changes their color. We want to avoid it. Here we only handle indicator when is above or below item.

    QRect indicator = option->rect;
    indicator.setHeight(2);
    if (isIndicatorAboveItem)
        indicator.adjust(0, -1, 0, -1);

    painter->fillRect(indicator, QColor(4, 100, 150));
}

FolderTreeView::FolderTreeView(QWidget *parent) : QTreeView(parent), delegate(this)
{
    setContextMenuPolicy(Qt::CustomContextMenu);
    viewport()->setAttribute(Qt::WA_Hover);
    QObject::connect(this, &FolderTreeView::customContextMenuRequested, this,
                     &FolderTreeView::onCustomContextMenuRequested);
    QObject::connect(&delegate, &FolderTreeDelegate::contextMenuRequested, this,
                     &FolderTreeView::customContextMenuRequested);

    setItemDelegate(&delegate);

    setStyleSheet("FolderTreeView{border: none; selection-color: transparent; selection-background-color: transparent; "
                  "background-color: transparent;}"
                  "QTreeView::branch { background: transparent; } "
                  "QScrollBar::vertical {width: 10px; border-radius: 5px; margin-left: 2px;}");
    myStyle = new FolderTreeViewProxyStyle;
    setStyle(myStyle);
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
    QMenu *menu = new QMenu(this);

    if (!index.isValid() or index.data(FolderTreeModelRole::Id).toInt() >= SpecialFolderId::UserFolder)
    {
        QAction *createSubfolder =
            new QAction(QIcon(":/images/addBlack.png"), index.isValid() ? "Create subfolder" : "Create new folder");
        QObject::connect(createSubfolder, &QAction::triggered, this,
                         [this, index]() { this->model()->insertRow(0, index); });
        menu->addAction(createSubfolder);
    }

    if (index.isValid())
    {

        if (index.data(FolderTreeModelRole::Id).toInt() >= SpecialFolderId::UserFolder)
        {
            QAction *renameFolder = new QAction(QIcon(":/images/renameFolder.png"), "Rename folder");
            QObject::connect(renameFolder, &QAction::triggered, this, [this, index]() { this->edit(index); });
            menu->addAction(renameFolder);
        }
        ColorPicker *colorPicker = new ColorPicker(nullptr);
        colorPicker->insertColor(QColor(255, 255, 255), 0, true);
        colorPicker->setColumnCount(colorPicker->getButtonCount());
        colorPicker->setCancelButtonVisible(false);
        colorPicker->setFixedSize(QSize(200, 50));
        colorPicker->setStyleSheet("");
        QObject::connect(colorPicker, &ColorPicker::colorSelected, this, [index, menu, this](const QColor &color) {
            FolderTreeItem *folder = static_cast<FolderTreeItem *>(index.internalPointer());
            this->model()->setData(index, color, FolderTreeModelRole::Color);
            folder->data.setColor(color);
            menu->close();
        });

        QWidgetAction *changeColorAction = new QWidgetAction(nullptr);
        changeColorAction->setDefaultWidget(colorPicker);
        QMenu *submenu = menu->addMenu(QIcon(":/images/palette.png"), "Change color");
        submenu->addAction(changeColorAction);

        if (index.data(FolderTreeModelRole::Id).toInt() >= SpecialFolderId::UserFolder)
        {
            QAction *deleteFolder = new QAction(QIcon(":/images/delete.png"), "Delete folder");
            QObject::connect(deleteFolder, &QAction::triggered, this, [this, index]() { this->deleteFolder(index); });
            menu->addAction(deleteFolder);
        }
    }
    menu->popup(mapToGlobal(pos));
}

void FolderTreeView::deleteFolder(const QModelIndex &index)
{
    auto reply = makeSureIfUserWantToDeleteFolder();
    if (reply == QMessageBox::No)
        return;

    model()->removeRow(index.row(), index.parent());
}

QPixmap FolderTreeView::renderDragPixmap(const QModelIndexList &indexes, QPoint *hotSpot)
{
    constexpr QSize leftPartOfItem = QSize(20, 0); // Delegate draws item outside of given place
    constexpr float decraseFactor = 0.9;
    const QSize sizeOfPixmap = (visualRect(indexes.at(0)).size() + QSize(20, 0)) * decraseFactor;
    QPixmap pixmap(sizeOfPixmap);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    QStyleOptionViewItem option;
    initViewItemOption(&option);
    option.rect.setSize(sizeOfPixmap);
    option.rect.setLeft(option.rect.left() + leftPartOfItem.width() * decraseFactor);
    option.state |= QStyle::State_Selected;
    if (isExpanded(indexes.at(0)))
        option.state |= QStyle::State_Open;

    delegate.paint(&painter, option, selectedIndexes().at(0));
    *hotSpot = ((mapFromGlobal(QCursor::pos()) - visualRect(indexes.at(0)).topLeft() +
                 QPoint(leftPartOfItem.width(), leftPartOfItem.height())) *
                decraseFactor);

    return pixmap;
}

QMessageBox::StandardButton FolderTreeView::makeSureIfUserWantToDeleteFolder()
{
    return QMessageBox::question(this, "Delete folder?",
                                 "Are you sure you want to delete this folder? All subfolders will be "
                                 "removed and notes will me moved to trash.");
}

void FolderTreeView::startDrag(Qt::DropActions supportedActions)
{
    QModelIndexList indexes = selectedIndexes();

    if (indexes.count() > 0)
    {
        QMimeData *data = model()->mimeData(indexes);
        if (!data)
            return;

        QDrag *drag = new QDrag(this);
        QPoint hotSpot;
        QPixmap dragPixmap = renderDragPixmap(indexes, &hotSpot);
        drag->setPixmap(dragPixmap);
        drag->setMimeData(data);
        drag->setHotSpot(hotSpot);
        dropTooltipOffset = QPoint(0, qMax(dragPixmap.height() - drag->hotSpot().y() + 3, 6));
        drag->exec(supportedActions);
    }
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

    dropTooltip->move(
        QWidget::window()->mapFromGlobal(this->mapToGlobal(event->position().toPoint()) + dropTooltipOffset));
    updateTextOfDropTooltip(dropIndex, *event->mimeData());
    updateSizeOfDropTooltip();
    updateTextFormatingOfDropTooltip(dropIndex);
    dropTooltip->setVisible(true);
}

void FolderTreeView::updateTextOfDropTooltip(const QModelIndex &dropIndex, const QMimeData &mimeData)
{
    FolderTreeModel *folderModel = static_cast<FolderTreeModel *>(model());
    if (dropIndicatorPosition() == OnItem)
    {
        if (mimeData.hasFormat(FolderMimeData::type) or
            !folderModel->areAllDragingNotesInFolderIndex(dropIndex, &mimeData))
        {
            dropTooltip->setText("Move to: " + dropIndex.data().toString());
        }
        else
            dropTooltip->setText("Notes already in this folder");
    }
    else if (dropIndicatorPosition() == BelowItem)
    {
        dropTooltip->setText("Move below: " + dropIndex.data().toString());
    }
    else if (dropIndicatorPosition() == AboveItem)
    {
        if (dropIndex.row() <= 0)
            dropTooltip->setText("Move above: " + model()->index(0, 0, dropIndex.parent()).data().toString());
        else
            dropTooltip->setText("Move below: " +
                                 model()->index(dropIndex.row() - 1, 0, dropIndex.parent()).data().toString());
    }
}

void FolderTreeView::updateSizeOfDropTooltip()
{
    QSize sizeOftooltip = dropTooltip->fontMetrics().size(Qt::TextSingleLine, dropTooltip->text());
    sizeOftooltip.setWidth(qMin(sizeOftooltip.width(), 300));
    QString elidedText =
        dropTooltip->fontMetrics().elidedText(dropTooltip->text(), Qt::ElideRight, sizeOftooltip.width());
    dropTooltip->setText(elidedText);
    sizeOftooltip = sizeOftooltip.grownBy(QMargins(10, 5, 10, 10));
    dropTooltip->resize(sizeOftooltip);
}

void FolderTreeView::updateTextFormatingOfDropTooltip(const QModelIndex &dropIndex)
{
    QString tooltipText = dropTooltip->text();

    if (dropIndex.data(FolderTreeModelRole::Id) == SpecialFolderId::TrashFolder and dropIndicatorPosition() == OnItem)
    {
        tooltipText.insert(tooltipText.size(), "</font>");
        tooltipText.insert(0, "<font color = red>");
    }
    else if (tooltipText.indexOf("Move to:") == 0)
    {
        tooltipText.insert(8, "</font>");
        tooltipText.insert(0, "<font color = #1741d5>");
    }
    else if (tooltipText.indexOf("Move above:") == 0 or tooltipText.indexOf("Move below:") == 0)
    {
    }
    else if (tooltipText == "Notes already in this folder")
    {
        tooltipText.insert(tooltipText.size(), "</font>");
        tooltipText.insert(0, "<font color = #ffa500>");
    }

    dropTooltip->setText(tooltipText);
}

void FolderTreeView::dropEvent(QDropEvent *event)
{
    bool handleDrop = true;
    dropTooltip->setVisible(false);

    QModelIndex dropIndex = indexAt(event->position().toPoint());
    if (dropIndex.isValid() and dropIndex.data(FolderTreeModelRole::Id) == SpecialFolderId::TrashFolder and
        dropIndicatorPosition() == OnItem)
    {
        auto reply = makeSureIfUserWantToDeleteFolder();

        if (reply == QMessageBox::No)
            handleDrop = false;
    }
    if (handleDrop)
        QTreeView::dropEvent(event);

    delegate.setDropIndex(QModelIndex());
    myStyle->drawIndicator = false;
}

void FolderTreeView::dragMoveEvent(QDragMoveEvent *event)
{
    QTreeView::dragMoveEvent(event);
    QModelIndex dropIndex = indexAt(event->position().toPoint());
    delegate.setDropIndex(dropIndex);

    if (event->isAccepted())
    {
        QModelIndex dropIndex = indexAt(event->position().toPoint());

        if (dropIndicatorPosition() == OnItem)
            delegate.setDropIndex(dropIndex);
        else
            delegate.setDropIndex(QModelIndex());

        myStyle->drawIndicator = true;
        if (dropIndicatorPosition() == AboveItem)
            myStyle->isIndicatorAboveItem = true;
        else
            myStyle->isIndicatorAboveItem = false;

        dropTooltip->setVisible(false);
        updateDropTooltip(event);
    }
    else
    {
        delegate.setDropIndex(QModelIndex());
        myStyle->drawIndicator = false;
        dropTooltip->setVisible(false);
    }

    viewport()->update();
}

void FolderTreeView::dragLeaveEvent(QDragLeaveEvent *event)
{
    QAbstractItemView::dragLeaveEvent(event);
    delegate.setDropIndex(QModelIndex());
    myStyle->drawIndicator = false;
    dropTooltip->setVisible(false);
}

void FolderTreeView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(NoteMimeData::type))
    {
        dropTooltipOffset = QPoint(5, 5);
        setDragDropOverwriteMode(true);
    }
    else
        setDragDropOverwriteMode(false);

    QTreeView::dragEnterEvent(event);
}
