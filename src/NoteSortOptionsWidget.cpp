#include "NoteSortOptionsWidget.h"
#include "./ui_NoteSortOptionsWidget.h"

#include "AboutWindow.h"
#include "NoteListModelRole.h"
#include <QPainter>
#include <QPainterPath>
#include <QRegion>

NoteSortOptionsWidget::NoteSortOptionsWidget(int currentSortRole, Qt::SortOrder currentSortOrder, QWidget *parent,
                                             Qt::WindowFlags windowsFlags)
    : QWidget(parent, windowsFlags), ui(new Ui::NoteSortOptionsWidget)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_TranslucentBackground);

    indexToSortRoleMap[0] = NoteListModelRole::CreationTime;
    indexToSortRoleMap[1] = NoteListModelRole::ModificationTime;
    indexToSortRoleMap[2] = NoteListModelRole::Title;

    indexToSordOrderMap[0] = Qt::AscendingOrder;
    indexToSordOrderMap[1] = Qt::DescendingOrder;

    setStyleSheet("QWidget#NoteSortOptionsWidget{border: none}; QLabel{font-family: verdana;}");

    ui->sortOrderComboBox->setCurrentIndex(indexToSordOrderMap.key(currentSortOrder));
    ui->sortRoleComboBox->setCurrentIndex(indexToSortRoleMap.key(currentSortRole));

    QObject::connect(ui->sortRoleComboBox, &QComboBox::currentIndexChanged, this,
                     [this](int index) { emit this->newSortRoleSelected(indexToSortRoleMap[index]); });

    QObject::connect(ui->sortOrderComboBox, &QComboBox::currentIndexChanged, this,
                     [this](int index) { emit this->newSortOrderSelected(indexToSordOrderMap[index]); });

    QObject::connect(ui->aboutNotes, &QPushButton::clicked, this, [this]() {
        auto aboutWindow = new AboutWindow(this);
        aboutWindow->show();
    });
}

void NoteSortOptionsWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(rect(), 20, 20);
    painter.fillPath(path, Qt::white);
    painter.setPen(QPen(Qt::black, 1));
    painter.drawPath(path);
}
