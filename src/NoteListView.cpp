#include "NoteListView.h"
#include "NoteListModel.h"
#include <QAbstractProxyModel>
#include <QApplication>
#include <QEvent>
#include <QHBoxLayout>
#include <QMenu>
#include <QMouseEvent>
#include <QScrollBar>
#include <qmessagebox.h>

NoteListView::NoteListView(QWidget *parent)
    : QListView(parent), editor(nullptr), noteListDelegate(this), inSelectingState(false), isDragSelecting(false),
      wasCtrlPressedWhileStartingDragSelecting(false)
{
    setSelectionMode(QAbstractItemView::NoSelection); // there is implemented custom selection
    setMouseTracking(true);
    setItemDelegate(&noteListDelegate);
    setAutoScroll(false);
    setVerticalScrollMode(ScrollMode::ScrollPerPixel);
    verticalScrollBar()->setSingleStep(20);
    QObject::connect(&noteListDelegate, &NoteListDelegate::newEditorCreated, this, &NoteListView::onNewEditorCreated);
    QObject::connect(this, &QListView::customContextMenuRequested, this, &NoteListView::onCustomContextMenuRequested);

    setUniformItemSizes(true);
    setGridSize(QSize(220, 220));
    setResizeMode(ResizeMode::Adjust);
    setViewMode(ViewMode::IconMode);
    int widthOfScrollbar = 14;
    QString style =
        QString(
            "QListView{border: none;} QScrollBar { width: %1px; background: transparent; } "
            "QScrollBar::handle:vertical:hover {background: rgba(40, 40, 40, 0.5);} "
            "QScrollBar::handle:vertical:pressed {background: rgba(40, 40, 40, 0.5);}"
            "QScrollBar::handle:vertical {border-radius: 4px;background: rgba(100, 100, 100, 0.5);min-height: 20px;}"
            "QScrollBar::vertical {border-radius: 6px;width: 10px;color: rgba(255, 255, 255, 0);}"
            "QScrollBar::add-line:vertical {width: 0px; height: 0px;subcontrol-position: bottom;subcontrol-origin: "
            "margin;}"
            "QScrollBar::sub-line:vertical {width: 0px;height: 0px;subcontrol-position: top;subcontrol-origin: "
            "margin;}")
            .arg(widthOfScrollbar);

    verticalScrollBar()->resize(widthOfScrollbar, 0);
    setStyleSheet(style);

    selectionMenu = new NoteListViewSelectionMenu(this);
    QObject::connect(selectionMenu, &NoteListViewSelectionMenu::colorSelected, this,
                     &NoteListView::changeColorOfSelectedNotes);
    QObject::connect(selectionMenu, &NoteListViewSelectionMenu::deleteButtonClicked, this,
                     &NoteListView::removeSelectedNotes);
    QObject::connect(selectionMenu, &NoteListViewSelectionMenu::pinButtonClicked, this,
                     &NoteListView::toogleIsPinnedOfSelectedNotes);
    QObject::connect(selectionMenu, &NoteListViewSelectionMenu::cancelButtonClicked, this,
                     [this]() { this->setInSelectingState(false); });
    setInSelectingState(false);
}

void NoteListView::removeNote(const QModelIndex &index)
{
    const QString message = [index]() {
        if (index.data(NoteListModel::isInTrash).toBool())
            return "Are you sure you want to delete this note? You won't be able to recover deleted note";
        else
            return "Are you sure you want to remove this note? You will be able to restore it from trash folder";
    }();

    auto reply = QMessageBox::question(this, "Delete note?", message);
    if (reply == QMessageBox::No)
        return;

    model()->removeRow(currentIndexWithEditor.row());
}

void NoteListView::onNewEditorCreated(NoteButton *editor, const QModelIndex &index)
{
    this->editor = editor;
    editor->installEventFilter(this);
    auto children = editor->findChildren<QWidget *>();
    for (auto const &child : children)
        child->installEventFilter(this);

    editor->setIsSelected(selectionModel()->isSelected(index));
    if (inSelectingState and !editor->getIsPinned())
        editor->setPinCheckboxVisible(false);

    QObject::connect(editor, &NoteButton::deleteNote, this,
                     [this, index]() { this->removeNote(this->currentIndex()); });
    QObject::connect(editor, &NoteButton::clicked, this, [this, index]() { emit this->noteSelected(index); });
    QObject::connect(editor, &NoteButton::pinCheckboxToogled, this,
                     [this, editor, index]() { noteListDelegate.setModelData(editor, this->model(), index); });
    QObject::connect(editor, &NoteButton::colorChanged, this,
                     [this, editor, index]() { noteListDelegate.setModelData(editor, this->model(), index); });
}

NoteListModel *NoteListView::getSourceModelAtTheBottom() const
{
    // We know that at the bottom there is NoteListModel.
    // If we want to access it, we need to find it, because proxy model can be set to view
    QAbstractItemModel *currentModel = model();
    forever
    {
        if (NoteListModel *noteModel = qobject_cast<NoteListModel *>(currentModel))
            return noteModel;

        if (QAbstractProxyModel *proxyModel = qobject_cast<QAbstractProxyModel *>(currentModel))
            currentModel = proxyModel->sourceModel();
        else
            return nullptr;
    }
}

QModelIndex NoteListView::mapIndexToSourceModelAtTheBott(const QModelIndex &index) const
{
    // We know that at the bottom there is NoteListModel.
    // Because indexes which view uses can be from proxy model,
    // we need to map it if we want to access directly NoteListModel
    QAbstractItemModel *currentModel = model();
    QModelIndex currentIndex = index;
    while (QAbstractProxyModel *proxyModel = qobject_cast<QAbstractProxyModel *>(currentModel))
    {
        currentIndex = proxyModel->mapToSource(currentIndex);
        currentModel = proxyModel->sourceModel();
    }

    return currentIndex;
}

bool NoteListView::isTrashFolderLoaded()
{
    NoteListModel *noteModel = getSourceModelAtTheBottom();
    if (noteModel)
        return noteModel->getCurrentSelectedFolderId() == SpecialFolderId::TrashFolder;

    return false;
}

void NoteListView::onCustomContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = indexAt(pos);
    QMenu *menu = new QMenu(this);

    if (!isTrashFolderLoaded())
    {
        QAction *createNote = new QAction("Create new note");
        QObject::connect(createNote, &QAction::triggered, this,
                         [this, index]() { this->model()->insertRow(0, index); });
        menu->addAction(createNote);
        if (index.isValid())
        {
            if (!index.data(NoteListModel::isPinned).toBool())
            {
                QAction *pinNote = new QAction("Pin note");
                QObject::connect(pinNote, &QAction::triggered, this,
                                 [this, index]() { this->model()->setData(index, true, NoteListModel::isPinned); });
                menu->addAction(pinNote);
            }
            else
            {
                QAction *unpinNote = new QAction("Unpin note");
                QObject::connect(unpinNote, &QAction::triggered, this,
                                 [this, index]() { this->model()->setData(index, false, NoteListModel::isPinned); });
                menu->addAction(unpinNote);
            }
        }
    }
    else if (index.isValid())
    {
        QAction *restoreNote = new QAction("Restore note");
        QObject::connect(restoreNote, &QAction::triggered, this,
                         [this, index]() { this->onRestoreNoteFromTrashRequested(index); });
        menu->addAction(restoreNote);
    }

    if (index.isValid())
    {
        QAction *deleteNote = new QAction("Delete note");
        QObject::connect(deleteNote, &QAction::triggered, this, [this, index]() { removeNote(index); });
        menu->addAction(deleteNote);
    }

    menu->exec(mapToGlobal(pos));
}

void NoteListView::onRestoreNoteFromTrashRequested(const QModelIndex &index)
{
    NoteListModel *noteModel = getSourceModelAtTheBottom();
    QModelIndex noteModelIndex = mapIndexToSourceModelAtTheBott(index);
    if (noteModel)
        noteModel->restoreNoteFromTrash(noteModelIndex);
}

void NoteListView::removeSelectedNotes()
{
    const QString message = [this]() {
        if (this->isTrashFolderLoaded())
            return "Are you sure you want to delete selected notes? You won't be able to recover deleted notes";
        else
            return "Are you sure you want to remove selected notes? You will be able to restore them from trash folder";
    }();

    auto reply = QMessageBox::question(this, "Delete selected notes?", message);
    if (reply == QMessageBox::No)
        return;

    auto selected = selectionModel()->selectedIndexes();
    std::sort(selected.begin(), selected.end(),
              [](const QModelIndex &first, const QModelIndex &second) { return first.row() > second.row(); });

    for (auto it = selected.constBegin(); it != selected.constEnd(); ++it)
    {
        model()->removeRow(it->row());
    }
}

void NoteListView::changeColorOfSelectedNotes(const QColor &newColor)
{
    auto selected = selectionModel()->selectedIndexes();
    for (auto const &index : selected)
    {
        model()->setData(index, newColor, NoteListModel::Color);
    }
}

void NoteListView::toogleIsPinnedOfSelectedNotes()
{
    auto selected = selectionModel()->selectedIndexes();
    QVector<QPersistentModelIndex> persistentSelected;
    for (auto const &index : selected)
        persistentSelected.append(index);

    bool allSelectedNotesArePinned = std::find_if(selected.begin(), selected.end(), [](const QModelIndex &index) {
                                         return !index.data(NoteListModel::isPinned).toBool();
                                     }) == selected.end();

    bool pinNotes;
    if (allSelectedNotesArePinned)
        pinNotes = false;
    else
        pinNotes = true;

    for (auto const &index : persistentSelected)
    {
        model()->setData(index, pinNotes, NoteListModel::isPinned);
    }
}

bool NoteListView::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == verticalScrollBar() or watched == horizontalScrollBar())
        return QListView::eventFilter(watched, event);

    if (QApplication::keyboardModifiers() & Qt::ControlModifier or inSelectingState)
    {
        if (event->type() == QEvent::MouseButtonPress or event->type() == QEvent::MouseButtonDblClick)
            return true;
        if (event->type() == QEvent::MouseButtonRelease)
        {
            setInSelectingState(true);
            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
            QPoint positionInView = this->mapFromGlobal(mouseEvent->globalPosition().toPoint());
            QModelIndex clickedIndex = indexAt(positionInView);
            selectionModel()->select(clickedIndex, QItemSelectionModel::Toggle);
            if (selectionModel()->selectedIndexes().count() == 0)
                setInSelectingState(false);

            return true;
        }
    }
    return QListView::eventFilter(watched, event);
}

void NoteListView::selectNotes(const QRect &rubberBand)
{
    auto currentState = state();
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setState(State::DragSelectingState);
    setSelection(rubberBand, QItemSelectionModel::Select);

    if (!selectionModel()->selection().isEmpty())
        setInSelectingState(true);

    setSelectionMode(QAbstractItemView::NoSelection);
    setState(currentState);
}

QPoint NoteListView::getOffsetOfViewport() const
{
    return QPoint(horizontalOffset(), verticalOffset());
}

void NoteListView::startDragSelecting(QPoint startPoint)
{
    isDragSelecting = true;
    startPoint.setY(qBound(-1, startPoint.y(), height()));
    dragSelectingInitialPoint = startPoint + QPoint(horizontalOffset(), verticalOffset());
    wasCtrlPressedWhileStartingDragSelecting = QApplication::keyboardModifiers() & Qt::ControlModifier;
}

void NoteListView::updateMousePositionOfDragSelecting(QPoint mousePositionPoint)
{
    if (!isDragSelecting)
        return;

    mousePositionPoint.setY(qBound(-1, mousePositionPoint.y(), height()));
    QRect selectionBound(dragSelectingInitialPoint - QPoint(horizontalOffset(), verticalOffset()), mousePositionPoint);

    if (!wasCtrlPressedWhileStartingDragSelecting)
        clearSelection();

    selectNotes(selectionBound.normalized());
}

void NoteListView::endDragSelecting()
{
    isDragSelecting = false;
}

void NoteListView::resizeEvent(QResizeEvent *event)
{
    QListView::resizeEvent(event);
    int widthOfSelectionMenu = 200;
    selectionMenu->setGeometry((width() - widthOfSelectionMenu - verticalScrollBar()->width()) / 2,
                               viewport()->height() - 75, widthOfSelectionMenu, 50);
}

void NoteListView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    if (!selectionModel())
        return;

    int selectedNotesCount = selectionModel()->selectedIndexes().count();
    selectionMenu->setSelectedNotesCount(selectedNotesCount);
    if (selectedNotesCount == 0)
        setInSelectingState(false);

    if (selected.contains(currentIndexWithEditor) && editor)
        editor->setIsSelected(true);

    if (deselected.contains(currentIndexWithEditor) && editor)
        editor->setIsSelected(false);

    QListView::selectionChanged(selected, deselected);
}

void NoteListView::setModel(QAbstractItemModel *model)
{
    QListView::setModel(model);
    QObject::connect(model, &QAbstractItemModel::modelReset, this, [this]() { setInSelectingState(false); });
}

bool NoteListView::viewportEvent(QEvent *event)
{
    bool result = QListView::viewportEvent(event);
    if (event->type() == QEvent::MouseButtonPress or event->type() == QEvent::MouseMove or
        event->type() == QEvent::MouseButtonRelease)
    {
        event->ignore();
    }
    return result;
}

void NoteListView::verticalScrollbarValueChanged(int value)
{
    QListView::verticalScrollbarValueChanged(value);
    updateMousePositionOfDragSelecting(mapFromGlobal(QCursor::pos()));
}

void NoteListView::updateEditor()
{
    QPoint mousePosition = mapFromGlobal(QCursor::pos());
    const QModelIndex index = indexAt(mousePosition);

    if (currentIndexWithEditor == index)
        return;

    if (currentIndexWithEditor.isValid())
    {
        closePersistentEditor(currentIndexWithEditor);
        editor = nullptr;
    }

    if (index.isValid())
        openPersistentEditor(index);

    currentIndexWithEditor = index;
}

bool NoteListView::event(QEvent *event)
{
    if (event->type() == QEvent::MouseMove || event->type() == QEvent::Paint)
        updateEditor();

    return QListView::event(event);
}

bool NoteListView::getInSelectingState() const
{
    return inSelectingState;
}

void NoteListView::setInSelectingState(bool newInSelectingState)
{
    inSelectingState = newInSelectingState;
    selectionMenu->setVisible(newInSelectingState);
    if (selectionModel())
        selectionMenu->setSelectedNotesCount(selectionModel()->selectedIndexes().count());

    if (!newInSelectingState)
    {
        selectionMenu->setColorPickerVisible(false);
        if (selectionModel() and !selectionModel()->selectedIndexes().empty())
            selectionModel()->clearSelection();
        if (editor)
        {
            editor->setPinCheckboxVisible(true);
            editor->setIsSelected(false);
        }
    }
    else
    {
        if (editor and !editor->getIsPinned())
            editor->setPinCheckboxVisible(false);
    }
}

int NoteListView::getHowManyNotesCanFitInRow(int width) const
{
    return (width - verticalScrollBar()->geometry().width() - 1) / gridSize().width();
}

int NoteListView::getHowManyNotesAreDisplayed() const
{
    return model()->rowCount();
}

void NoteListView::setMinWidthToFitNotesInRow(int numberOfNotesToFitInOneRow)
{
    resize(numberOfNotesToFitInOneRow * gridSize().width() + verticalScrollBar()->geometry().width() + 1, height());
}
