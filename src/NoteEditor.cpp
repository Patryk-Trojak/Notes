#include "NoteEditor.h"
#include "./ui_NoteEditor.h"
#include "NoteListModelRole.h"
#include <QColorDialog>
#include <QDateTime>
#include <QFontInfo>
#include <QResizeEvent>
#include <QTextDocument>
#include <QTextList>

NoteEditor::NoteEditor(const QModelIndex &editingNote, QWidget *parent) : QWidget(parent), ui(new Ui::NoteEditor)
{
    ui->setupUi(this);
    int widthOfScrollbar = 14;
    QString style = QString("QWidget#NoteEditor{border-style: solid; border-color: transparent; border-width:10px; "
                            "border-radius:30px;} "
                            "QWidget{ background-color: %1;}"
                            "QLineEdit{background-color: transparent; border: none;}"
                            "QTextEdit{background-color: transparent; border: none;}")
                        .arg(editingNote.data(NoteListModelRole::Color).value<QColor>().name());

    setStyleSheet(style);
    setAttribute(Qt::WA_StyledBackground, true);
    ui->creationTime->setStyleSheet("QLabel{color: black}");
    ui->modificationTime->setStyleSheet("QLabel{color: black}");
    ui->titleEdit->setText(editingNote.data(NoteListModelRole::Title).toString());
    ui->contentEdit->setHtml(editingNote.data(NoteListModelRole::Content).toString());
    setCreationTime(editingNote.data(NoteListModelRole::CreationTime).toDateTime());
    setModificationTime(editingNote.data(NoteListModelRole::ModificationTime).toDateTime());

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
                     [this]() { emit contentChanged(this->ui->contentEdit->toHtml()); });
    QObject::connect(ui->contentEdit, &QTextEdit::textChanged, this,
                     [this]() { setModificationTime(QDateTime::currentDateTime()); });

    QObject::connect(ui->boldCheckBox, &QCheckBox::clicked, this, &NoteEditor::switchTextBold);
    QObject::connect(ui->italicCheckBox, &QCheckBox::clicked, this, &NoteEditor::switchTextItalic);
    QObject::connect(ui->contentEdit, &QTextEdit::currentCharFormatChanged, this,
                     &NoteEditor::onCurrentCharFormatChanged);
    QObject::connect(ui->underlineCheckBox, &QCheckBox::clicked, this, &NoteEditor::switchTextUnderline);
    QObject::connect(ui->strikeOutCheckBox, &QCheckBox::clicked, this, &NoteEditor::switchTextStrikeOut);
    QObject::connect(ui->textColorButton, &QPushButton::clicked, this, &NoteEditor::setTextColor);
    QObject::connect(ui->textBackgroundColorButton, &QPushButton::clicked, this, &NoteEditor::setTextBackgroundColor);

    onCurrentCharFormatChanged(ui->contentEdit->currentCharFormat());
}

NoteEditor::~NoteEditor()
{
    delete ui;
}

void NoteEditor::setResourceLoader(const NoteContentEdit::ResourceLoader &resourceProvider)
{
    ui->contentEdit->setResourceLoader(resourceProvider);
}

void NoteEditor::setResourceSaver(const NoteContentEdit::ResourceSaver &resourceSaver)
{
    ui->contentEdit->setResourceSaver(resourceSaver);
}

void NoteEditor::setCreationTime(const QDateTime &creationTime)
{
    ui->creationTime->setText("Created: " + creationTime.toString("dd.MM.yy HH:mm"));
}

void NoteEditor::setModificationTime(const QDateTime &modificationTime)
{
    ui->modificationTime->setText("Modified: " + modificationTime.toString("dd.MM.yy HH:mm"));
}

void NoteEditor::switchTextBold()
{
    QTextCharFormat fmt;
    fmt.setFontWeight(ui->boldCheckBox->isChecked() ? QFont::Bold : QFont::Normal);
    ui->contentEdit->mergeCurrentCharFormat(fmt);
}

void NoteEditor::switchTextItalic()
{
    QTextCharFormat fmt;
    fmt.setFontItalic(ui->italicCheckBox->isChecked());
    ui->contentEdit->mergeCurrentCharFormat(fmt);
}

void NoteEditor::switchTextUnderline()
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(ui->underlineCheckBox->isChecked());
    ui->contentEdit->mergeCurrentCharFormat(fmt);
}

void NoteEditor::switchTextStrikeOut()
{
    QTextCharFormat fmt;
    fmt.setFontStrikeOut(ui->strikeOutCheckBox->isChecked());
    ui->contentEdit->mergeCurrentCharFormat(fmt);
}

void NoteEditor::setTextColor()
{
    QColor textColor = QColorDialog::getColor(ui->contentEdit->textColor(), this, "", QColorDialog::ShowAlphaChannel);
    ui->contentEdit->setTextColor(textColor);
}

void NoteEditor::setTextBackgroundColor()
{
    QColor textBackgroundColor =
        QColorDialog::getColor(ui->contentEdit->textBackgroundColor(), this, "", QColorDialog::ShowAlphaChannel);
    ui->contentEdit->setTextBackgroundColor(textBackgroundColor);
}

void NoteEditor::resizeEvent(QResizeEvent *event)
{
    QPoint rightTop = QPoint(event->size().width(), 0);
    closeButton->move(rightTop + QPoint(-closeButton->width() - 20, 20));
}

void NoteEditor::onCurrentCharFormatChanged(const QTextCharFormat &f)
{
    ui->boldCheckBox->setChecked(f.font().bold());
    ui->italicCheckBox->setChecked(f.font().italic());
    ui->underlineCheckBox->setChecked(f.font().underline());
    ui->strikeOutCheckBox->setChecked(f.font().strikeOut());
}
