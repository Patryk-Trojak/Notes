#ifndef SEARCHBAR_H
#define SEARCHBAR_H

#include <QLineEdit>

class SearchBar : public QLineEdit
{
    Q_OBJECT

  public:
    explicit SearchBar(QWidget *parent = nullptr);
};

#endif // SEARCHBAR_H
