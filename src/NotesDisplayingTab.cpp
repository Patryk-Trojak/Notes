#include "NotesDisplayingTab.h"
#include "ui_NotesDisplayingTab.h"
#include <AboutWindow.h>
#include <QScrollBar>
#include <QShortcut>
#include <QStringListModel>

NotesDisplayingTab::NotesDisplayingTab(NoteListModel &noteModel, PersistenceManager &persistenceManager,
                                       QWidget *parent)
    : QWidget(parent), ui(new Ui::NotesDisplayingTab), folderModel(persistenceManager, this), noteModel(noteModel),
      noteProxyModel(this)
{
    ui->setupUi(this);
    noteListView = new NoteListView(this);
    searchBar = new SearchBar(this);
    QObject::connect(ui->aboutNotes, &QPushButton::clicked, this, [this]() {
        auto aboutWindow = new AboutWindow(this);
        aboutWindow->show();
    });
    QObject::connect(ui->newNoteButton, &QPushButton::clicked, this, &NotesDisplayingTab::onNewNoteButtonPressed);
    QObject::connect(ui->sortByTitleButton, &QRadioButton::toggled, this,
                     &NotesDisplayingTab::onSortByTitleButtonToggled);
    QObject::connect(ui->sortByCreationDateButton, &QRadioButton::toggled, this,
                     &NotesDisplayingTab::onSortByCreationDateButtonToggled);
    QObject::connect(ui->sortByModificationDateButton, &QRadioButton::toggled, this,
                     &NotesDisplayingTab::onSortByModificationDateButtonToggled);
    QObject::connect(ui->sortInAscendingOrderButton, &QRadioButton::toggled, this,
                     &NotesDisplayingTab::onSortOrderButtonToggled);
    QObject::connect(ui->sortInDescendingOrder, &QRadioButton::toggled, this,
                     &NotesDisplayingTab::onSortOrderButtonToggled);

    QShortcut *createNewNoteShortcut = new QShortcut(QKeySequence(QKeySequence::New), this);
    QObject::connect(createNewNoteShortcut, &QShortcut::activated, this, &NotesDisplayingTab::onNewNoteButtonPressed);

    ui->folderTreeView->setModel(&folderModel);
    ui->folderTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(ui->folderTreeView, &FolderTreeView::newFolderSelected, this,
                     &NotesDisplayingTab::onNewFolderSelected);
    noteProxyModel.setSourceModel(&noteModel);
    noteProxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    noteProxyModel.setSortCaseSensitivity(Qt::CaseInsensitive);
    noteProxyModel.setSortRole(NoteListModel::CreationTime);
    noteProxyModel.sort(0, Qt::DescendingOrder);
    QObject::connect(searchBar, &QLineEdit::textChanged, this,
                     [this](const QString &searched) { this->noteProxyModel.setFilterFixedString(searched); });

    noteListView->setModel(&noteProxyModel);
    noteListView->setContextMenuPolicy(Qt::CustomContextMenu);
    QObject::connect(noteListView, &NoteListView::noteSelected, this, [this](const QModelIndex &index) {
        auto sourceIndex = this->noteProxyModel.mapToSource(index);
        emit this->enterEditingNote(sourceIndex);
    });

    onNewFolderSelected(ui->folderTreeView->getCurrentFolderSelectedId());
    QObject::connect(&folderModel, &FolderTreeModel::folderDeletedFromDatabase, &noteModel,
                     &NoteListModel::onFolderDeleted);

    layoutAllElementsWhichDependsOnNumberOfNotes();
    QObject::connect(&noteProxyModel, &NoteSortFilterProxyModel::rowsInserted, this,
                     &NotesDisplayingTab::layoutAllElementsWhichDependsOnNumberOfNotes);
    QObject::connect(&noteProxyModel, &NoteSortFilterProxyModel::rowsRemoved, this,
                     &NotesDisplayingTab::layoutAllElementsWhichDependsOnNumberOfNotes);
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

void NotesDisplayingTab::onSortByTitleButtonToggled()
{
    noteProxyModel.setSortRole(NoteListModel::Title);
}

void NotesDisplayingTab::onSortByCreationDateButtonToggled()
{
    noteProxyModel.setSortRole(NoteListModel::CreationTime);
}

void NotesDisplayingTab::onSortByModificationDateButtonToggled()
{
    noteProxyModel.setSortRole(NoteListModel::ModificationTime);
}

void NotesDisplayingTab::onSortOrderButtonToggled()
{
    Qt::SortOrder sortOrder = Qt::DescendingOrder;
    if (ui->sortInAscendingOrderButton->isChecked())
        sortOrder = Qt::AscendingOrder;

    noteProxyModel.sort(0, sortOrder);
}

void NotesDisplayingTab::onNewFolderSelected(int selectedFolderId)
{
    noteModel.onNewFolderSelected(selectedFolderId);
    searchBar->setText("");
    layoutAllElementsWhichDependsOnNumberOfNotes();
}

void NotesDisplayingTab::layoutSearchBar()
{
    int widthOfSearchBar = qMin(noteListView->width(), 600);
    widthOfSearchBar = qMax(widthOfSearchBar, 200);
    int leftOfSearchBar = noteListView->pos().x() +
                          (noteListView->width() - widthOfSearchBar - noteListView->verticalScrollBar()->width()) / 2;
    searchBar->setGeometry(leftOfSearchBar, 9, widthOfSearchBar, searchBar->height());
}

void NotesDisplayingTab::layoutNoteListView()
{
    int leftMargin = 9;
    int rightMargin = 9;
    int availableWidthForNoteView =
        ui->rightPanel->geometry().left() - ui->folderTreeView->geometry().right() - leftMargin - rightMargin;
    int numberOfNotesInRow = qMin(noteListView->getHowManyNotesCanFitInRow(availableWidthForNoteView),
                                  noteListView->getHowManyNotesAreDisplayed());
    noteListView->setMinWidthToFitNotesInRow(numberOfNotesInRow);

    int newLeftOfNoteView =
        ui->folderTreeView->geometry().right() + leftMargin +
        (availableWidthForNoteView - noteListView->width() + noteListView->verticalScrollBar()->width()) / 2;

    noteListView->move(newLeftOfNoteView, 51);
    noteListView->resize(noteListView->width(), height() - 60);
}

void NotesDisplayingTab::layoutAllElementsWhichDependsOnNumberOfNotes()
{
    layoutNoteListView();
    layoutSearchBar();
}

void NotesDisplayingTab::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    layoutAllElementsWhichDependsOnNumberOfNotes();
}
