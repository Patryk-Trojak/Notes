#include "SearchBar.h"

#include <QPainter>
#include <QPainterPath>

SearchBar::SearchBar(QWidget *parent) : QLineEdit(parent)
{
    QAction *action = new QAction(this);

    action->setIcon(QIcon(":/images/search"));
    addAction(action, QLineEdit::LeadingPosition);

    QAction *clear = new QAction(this);
    clear->setIcon(QIcon(":/images/clear"));
    addAction(clear, QLineEdit::TrailingPosition);
    clear->setVisible(false);

    QObject::connect(this, &QLineEdit::textChanged, clear, [this, clear]() {
        if (this->text() == "")
            clear->setVisible(false);
        else
            clear->setVisible(true);
    });
    QObject::connect(clear, &QAction::triggered, this, [this]() { this->setText(""); });
    setPlaceholderText("Search");

    setStyleSheet("QLineEdit{border: 1px solid rgba(100, 100, 100, 0.5); "
                  "border-radius:15px;} QLineEdit::focus{border-color: black;}");
}
