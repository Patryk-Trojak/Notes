#include "NotesDisplayingTab.h"
#include "ui_NotesDisplayingTab.h"
#include <AboutWindow.h>
#include <QShortcut>
#include <QStringListModel>
NotesDisplayingTab::NotesDisplayingTab(NoteListModel &noteModel, PersistenceManager &persistenceManager,
                                       QWidget *parent)
    : QWidget(parent), ui(new Ui::NotesDisplayingTab), folderModel(persistenceManager), noteModel(noteModel)
{
    ui->setupUi(this);
    QObject::connect(ui->aboutNotes, &QPushButton::clicked, this, [this]() {
        auto aboutWindow = new AboutWindow(this);
        aboutWindow->show();
        qInfo() << "Clicke!";
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
    QObject::connect(ui->searchBar, &QLineEdit::textChanged, this, &NotesDisplayingTab::filterSortButtonsByTitle);

    currentNoteButtonsSortingMethod = &NotesDisplayingTab::sortNoteButtonsByCreationDate;

    ui->folderTreeView->setModel(&folderModel);
    ui->folderTreeView->setContextMenuPolicy(Qt::CustomContextMenu);

    ui->noteListView->setModel(&noteModel);

    QObject::connect(ui->folderTreeView, &FolderTreeView::newFolderSelected, &noteModel,
                     &NoteListModel::onNewFolderSelected);
    QObject::connect(ui->noteListView, &NoteListView::noteSelected, this, &NotesDisplayingTab::enterEditingNote);
}

NotesDisplayingTab::~NotesDisplayingTab()
{
    delete ui;
}

void NotesDisplayingTab::onNewNoteButtonPressed()
{
    noteModel.createNewNote();
}

void NotesDisplayingTab::sortNoteButtons(std::function<bool(const QWidget *, const QWidget *)> compare)
{
}

void NotesDisplayingTab::sortNoteButtonsByTitle(bool ascendingOrder)
{
    sortNoteButtons([ascendingOrder](const QWidget *a, const QWidget *b) -> bool {
        if (ascendingOrder)
        {
            return static_cast<const NoteButton *>(a)->getTitle().toLower() <
                   static_cast<const NoteButton *>(b)->getTitle().toLower();
        }
        else
        {
            return static_cast<const NoteButton *>(a)->getTitle().toLower() >
                   static_cast<const NoteButton *>(b)->getTitle().toLower();
        }
    });
}

void NotesDisplayingTab::sortNoteButtonsByCreationDate(bool ascendingOrder)
{
    sortNoteButtons([ascendingOrder](const QWidget *a, const QWidget *b) -> bool {
        if (ascendingOrder)
        {
            return static_cast<const NoteButton *>(a)->getCreationTime() <
                   static_cast<const NoteButton *>(b)->getCreationTime();
        }
        else
        {
            return static_cast<const NoteButton *>(a)->getCreationTime() >
                   static_cast<const NoteButton *>(b)->getCreationTime();
        }
    });
}

void NotesDisplayingTab::sortNoteButtonsByModificationDate(bool ascendingOrder)
{
    sortNoteButtons([ascendingOrder](const QWidget *a, const QWidget *b) -> bool {
        if (ascendingOrder)
        {
            return static_cast<const NoteButton *>(a)->getModificationTime() <
                   static_cast<const NoteButton *>(b)->getModificationTime();
        }
        else
        {
            return static_cast<const NoteButton *>(a)->getModificationTime() >
                   static_cast<const NoteButton *>(b)->getModificationTime();
        }
    });
}

void NotesDisplayingTab::onSortByTitleButtonToggled()
{
    bool ascendingOrder = ui->sortInAscendingOrderButton->isChecked();
    sortNoteButtonsByTitle(ascendingOrder);
    currentNoteButtonsSortingMethod = &NotesDisplayingTab::sortNoteButtonsByTitle;
}

void NotesDisplayingTab::onSortByCreationDateButtonToggled()
{
    bool ascendingOrder = ui->sortInAscendingOrderButton->isChecked();
    sortNoteButtonsByCreationDate(ascendingOrder);
    currentNoteButtonsSortingMethod = &NotesDisplayingTab::sortNoteButtonsByCreationDate;
}

void NotesDisplayingTab::onSortByModificationDateButtonToggled()
{
    bool ascendingOrder = ui->sortInAscendingOrderButton->isChecked();
    sortNoteButtonsByModificationDate(ascendingOrder);
    currentNoteButtonsSortingMethod = &NotesDisplayingTab::sortNoteButtonsByModificationDate;
}

void NotesDisplayingTab::onSortOrderButtonToggled()
{
    currentNoteButtonsSortingMethod(this, ui->sortInAscendingOrderButton->isChecked());
}

void NotesDisplayingTab::filterSortButtonsByTitle(const QString &searched)
{
}
