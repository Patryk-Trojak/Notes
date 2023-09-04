#include "NotesDisplayingTab.h"
#include "ui_NotesDisplayingTab.h"
#include <AboutWindow.h>
#include <QMoveEvent>
#include <QScrollBar>
#include <QShortcut>
#include <QStringListModel>

NotesDisplayingTab::NotesDisplayingTab(NoteListModel &noteModel, PersistenceManager &persistenceManager,
                                       QWidget *parent)
    : QWidget(parent), ui(new Ui::NotesDisplayingTab), folderModel(persistenceManager, this), noteModel(noteModel),
      noteProxyModel(this), rubberBand(nullptr)
{
    ui->setupUi(this);
    ui->rightFrame->installEventFilter(this);
    QObject::connect(ui->rightFrame, &QWidget::customContextMenuRequested, this,
                     &NotesDisplayingTab::onRightFrameCustomMenuRequested);
    ui->newNoteButton->setAttribute(Qt::WA_StyledBackground, true);
    ui->splitter->handle(1)->setStyleSheet("border: none; background-color: rgb(191, 191, 191);");
    ui->splitter->handle(1)->setAttribute(Qt::WA_StyledBackground, true);
    ui->splitter->setHandleWidth(2);
    ui->splitter->setSizes({400, 2000});
    noteListView = new NoteListView(ui->rightFrame);
    searchBar = new SearchBar(ui->rightFrame);
    openNoteSortOptionsButton = new QPushButton(ui->rightFrame);
    openNoteSortOptionsButton->setIcon(QIcon(":/images/options.png"));
    openNoteSortOptionsButton->setStyleSheet("border: none; padding: 0px; margin: 0px;");
    openNoteSortOptionsButton->resize(22, 22);
    openNoteSortOptionsButton->setIconSize(QSize(22, 22));
    QObject::connect(openNoteSortOptionsButton, &QPushButton::clicked, this,
                     &NotesDisplayingTab::onOpenNoteSortOptionsButtonClicked);

    QObject::connect(ui->newNoteButton, &QPushButton::clicked, this, &NotesDisplayingTab::onNewNoteButtonPressed);

    QShortcut *createNewNoteShortcut = new QShortcut(QKeySequence(QKeySequence::New), this);
    QObject::connect(createNewNoteShortcut, &QShortcut::activated, this, &NotesDisplayingTab::onNewNoteButtonPressed);

    ui->folderTreeView->setModel(&folderModel);
    ui->folderTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui->folderTreeView, &FolderTreeView::newFolderSelected, this,
                     &NotesDisplayingTab::onNewFolderSelected);
    noteProxyModel.setSourceModel(&noteModel);
    noteProxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    noteProxyModel.setSortCaseSensitivity(Qt::CaseInsensitive);
    noteProxyModel.setSortRole(NoteListModelRole::CreationTime);
    noteProxyModel.sort(0, Qt::DescendingOrder);
    QObject::connect(searchBar, &QLineEdit::textChanged, this,
                     [this](const QString &searched) { this->noteProxyModel.setFilterFixedString(searched); });

    noteListView->setModel(&noteProxyModel);
    noteListView->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(noteListView, &NoteListView::noteSelected, this, [this](const QModelIndex &index) {
        auto sourceIndex = this->noteProxyModel.mapToSource(index);
        emit this->enterEditingNote(sourceIndex);
    });
    QObject::connect(noteListView->verticalScrollBar(), &QScrollBar::valueChanged, this,
                     &NotesDisplayingTab::updateRubberBand);
    onNewFolderSelected(ui->folderTreeView->getCurrentFolderSelectedId());
    QObject::connect(&folderModel, &FolderTreeModel::folderDeletedFromDatabase, &noteModel,
                     &NoteListModel::onFolderDeleted);
    QObject::connect(&folderModel, &FolderTreeModel::moveNotesToFolderRequested, &noteModel,
                     &NoteListModel::moveNotesToFolder);

    QObject::connect(&noteModel, &NoteListModel::notesAdded, &folderModel, &FolderTreeModel::onNotesAdded);
    QObject::connect(&noteModel, &NoteListModel::notesRemoved, &folderModel, &FolderTreeModel::onNotesRemoved);
    QObject::connect(&noteModel, &NoteListModel::notesMoved, &folderModel, &FolderTreeModel::onNotesMoved);

    QObject::connect(&noteProxyModel, &NoteSortFilterProxyModel::rowsInserted, this,
                     &NotesDisplayingTab::layoutAllElementsWhichDependsOnNumberOfNotes);
    QObject::connect(&noteProxyModel, &NoteSortFilterProxyModel::rowsRemoved, this,
                     &NotesDisplayingTab::layoutAllElementsWhichDependsOnNumberOfNotes);

    layoutAllElementsWhichDependsOnNumberOfNotes();
}

NotesDisplayingTab::~NotesDisplayingTab()
{
    delete ui;
}

void NotesDisplayingTab::onNewNoteButtonPressed()
{
    QModelIndex createdNoteIndex = noteModel.createNewNote();
    if (createdNoteIndex.isValid())
        emit enterEditingNote(createdNoteIndex);
}

void NotesDisplayingTab::onNewNoteSortRoleSelected(int newSortRole)
{
    noteProxyModel.setSortRole(newSortRole);
}

void NotesDisplayingTab::onNewNoteSortOrderSelected(Qt::SortOrder newSortOrder)
{
    noteProxyModel.sort(0, newSortOrder);
}

void NotesDisplayingTab::onNewFolderSelected(int selectedFolderId)
{
    noteModel.onNewFolderSelected(selectedFolderId);
    searchBar->setText("");
    layoutAllElementsWhichDependsOnNumberOfNotes();
}

void NotesDisplayingTab::onOpenNoteSortOptionsButtonClicked()
{
    NoteSortOptionsWidget *noteSortOptionsWidget =
        new NoteSortOptionsWidget(noteProxyModel.sortRole(), noteProxyModel.sortOrder(), ui->rightFrame, Qt::Popup);

    QPoint positionOfNoteSortOptionsWidget = this->ui->rightFrame->mapToGlobal(openNoteSortOptionsButton->pos());
    QSize sizeOfSortOptions(210, 100);
    noteSortOptionsWidget->setGeometry(positionOfNoteSortOptionsWidget.x() - sizeOfSortOptions.width(),
                                       positionOfNoteSortOptionsWidget.y() + openNoteSortOptionsButton->height() + 10,
                                       sizeOfSortOptions.width(), sizeOfSortOptions.height());

    QObject::connect(noteSortOptionsWidget, &NoteSortOptionsWidget::newSortRoleSelected, this,
                     &NotesDisplayingTab::onNewNoteSortRoleSelected);
    QObject::connect(noteSortOptionsWidget, &NoteSortOptionsWidget::newSortOrderSelected, this,
                     &NotesDisplayingTab::onNewNoteSortOrderSelected);

    noteSortOptionsWidget->show();
}

void NotesDisplayingTab::updateRubberBand()
{
    if (!rubberBand)
        return;

    QPoint currentOriginOfRubberBand(originOfRubberBandInNoteListView - noteListView->getOffsetOfViewport() +
                                     noteListView->pos());
    if (currentOriginOfRubberBand.y() < noteListView->y())
        currentOriginOfRubberBand.setY(noteListView->y());

    QPoint mousePosition = ui->rightFrame->mapFromGlobal(QCursor::pos());
    if (mousePosition.y() < noteListView->y())
    {
        mousePosition.setY(noteListView->y());
        if (mousePosition.y() == currentOriginOfRubberBand.y())
        {
            rubberBand->setGeometry(0, 0, 0, 0);
            return;
        }
    }

    rubberBand->setGeometry(QRect(currentOriginOfRubberBand, mousePosition).normalized());
}

void NotesDisplayingTab::onRightFrameCustomMenuRequested(const QPoint &pos)
{
    emit noteListView->customContextMenuRequested(noteListView->mapFromGlobal(ui->rightFrame->mapToGlobal(pos)));
}

void NotesDisplayingTab::layoutSearchBar()
{
    if (ui->splitter->sizes().at(1) == 0)
    {
        searchBar->setVisible(false);
        return;
    }
    else
        searchBar->setVisible(true);

    int widthOfSearchBar = qMin(noteListView->width() - 50, 600);
    widthOfSearchBar = qMax(widthOfSearchBar, 200);
    int leftOfSearchBar = noteListView->pos().x() +
                          (noteListView->width() - widthOfSearchBar - noteListView->verticalScrollBar()->width()) / 2;
    searchBar->setGeometry(leftOfSearchBar, 9, widthOfSearchBar, searchBar->height());
}

void NotesDisplayingTab::layoutOpenNoteSortOptionsButton()
{
    if (ui->splitter->sizes().at(1) == 0)
    {
        openNoteSortOptionsButton->setVisible(false);
        return;
    }
    else
        openNoteSortOptionsButton->setVisible(true);

    openNoteSortOptionsButton->move(searchBar->pos().x() + searchBar->width() + 5,
                                    searchBar->pos().y() +
                                        (searchBar->height() - openNoteSortOptionsButton->height()) / 2);
}

void NotesDisplayingTab::layoutNoteListView()
{
    int leftMargin = 7;
    int rightMargin = 7;
    int marginTop = 51;
    if (ui->splitter->sizes().at(1) == 0)
    {
        noteListView->setVisible(false);
        return;
    }
    else
        noteListView->setVisible(true);

    int availableWidthForNoteView = ui->rightFrame->width() - rightMargin - leftMargin;
    int numberOfNotesInRow = qMin(noteListView->getHowManyNotesCanFitInRow(availableWidthForNoteView),
                                  noteListView->getHowManyNotesAreDisplayed());
    noteListView->setMinWidthToFitNotesInRow(numberOfNotesInRow);

    int newLeftOfNoteView =
        leftMargin +
        (availableWidthForNoteView - noteListView->width() + noteListView->verticalScrollBar()->width()) / 2;

    noteListView->setGeometry(newLeftOfNoteView, marginTop, noteListView->width(),
                              ui->rightFrame->height() - marginTop);
}

void NotesDisplayingTab::layoutAllElementsWhichDependsOnNumberOfNotes()
{
    layoutNoteListView();
    layoutSearchBar();
    layoutOpenNoteSortOptionsButton();
}

void NotesDisplayingTab::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    layoutAllElementsWhichDependsOnNumberOfNotes();
}

bool NotesDisplayingTab::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == ui->rightFrame)
    {
        if (event->type() == QEvent::Resize or event->type() == QEvent::Move)
            layoutAllElementsWhichDependsOnNumberOfNotes();
    }
    return false;
}

void NotesDisplayingTab::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);

    if (!(event->button() & Qt::LeftButton))
        return;

    if (!ui->rightFrame->geometry().contains(event->pos()))
        return;

    if (rubberBand)
        delete rubberBand;

    originOfRubberBandInNoteListView =
        noteListView->mapFromGlobal(event->globalPosition().toPoint()) + noteListView->getOffsetOfViewport();
    if (originOfRubberBandInNoteListView.y() < noteListView->y())
        originOfRubberBandInNoteListView.setY(noteListView->getOffsetOfViewport().y());

    noteListView->startDragSelecting(noteListView->mapFromGlobal(event->globalPosition().toPoint()));
    rubberBand = new QRubberBand(QRubberBand::Rectangle, ui->rightFrame);
    wasRubberBandMoved = false;
    updateRubberBand();
    rubberBand->show();
}

void NotesDisplayingTab::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
    if (rubberBand)
    {
        if (!wasRubberBandMoved)
            noteListView->clearSelection();
        delete rubberBand;
        rubberBand = nullptr;
        noteListView->endDragSelecting();
    }
}

void NotesDisplayingTab::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    if (rubberBand)
    {
        updateRubberBand();
        wasRubberBandMoved = true;
        noteListView->updateMousePositionOfDragSelecting(
            noteListView->mapFromGlobal(event->globalPosition().toPoint()));
    }
}
