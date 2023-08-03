#include "NoteEditor.h"
#include "./ui_NoteEditor.h"
#include "NoteListModel.h"
#include <QResizeEvent>

NoteEditor::NoteEditor(const QModelIndex &editingNote, QWidget *parent) : QWidget(parent), ui(new Ui::NoteEditor)
{
    ui->setupUi(this);
    setStyleSheet(QString("QWidget{border-style: solid; border-color: transparent; border-width:10px; "
                          "border-radius:30px; background-color: %1;}")
                      .arg(editingNote.data(NoteListModel::Color).value<QColor>().name()));
    setAttribute(Qt::WA_StyledBackground, true);
    ui->creationTime->setStyleSheet("QLabel{color: black}");
    ui->modificationTime->setStyleSheet("QLabel{color: black}");
    ui->titleEdit->setText(editingNote.data(NoteListModel::Title).toString());
    ui->contentEdit->setText(editingNote.data(NoteListModel::Content).toString());
    setCreationTime(editingNote.data(NoteListModel::CreationTime).toDateTime());
    setModificationTime(editingNote.data(NoteListModel::ModificationTime).toDateTime());

    closeButton = new QPushButton(this);
    closeButton->setIcon(QIcon(":/images/clear.png"));
    closeButton->setStyleSheet("Margin: 0px; Padding: 0px; border-style: solid;");
    closeButton->setIconSize(QSize(20, 20));
    closeButton->resize(20, 20);

    QObject::connect(closeButton, &QPushButton::clicked, this, &NoteEditor::closeNoteRequested);
    QObject::connect(ui->titleEdit, &QLineEdit::textChanged, this, &NoteEditor::titleChanged);
    QObject::connect(ui->titleEdit, &QLineEdit::textChanged, this,
                     [this]() { setModificationTime(QDateTime::currentDateTime()); });
    QObject::connect(ui->contentEdit, &QTextEdit::textChanged, this,
                     [this]() { emit contentChanged(this->ui->contentEdit->toPlainText()); });
    QObject::connect(ui->contentEdit, &QTextEdit::textChanged, this,
                     [this]() { setModificationTime(QDateTime::currentDateTime()); });
}

NoteEditor::~NoteEditor()
{
    delete ui;
}

void NoteEditor::setCreationTime(const QDateTime &creationTime)
{
    ui->creationTime->setText("Created: " + creationTime.toString("dd.MM.yy HH:mm"));
}

void NoteEditor::setModificationTime(const QDateTime &modificationTime)
{
    ui->modificationTime->setText("Modified: " + modificationTime.toString("dd.MM.yy HH:mm"));
}

void NoteEditor::resizeEvent(QResizeEvent *event)
{
    QPoint rightTop = QPoint(event->size().width(), 0);
    closeButton->move(rightTop + QPoint(-closeButton->width() - 20, 20));
}
