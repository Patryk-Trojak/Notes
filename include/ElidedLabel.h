#ifndef ELIDEDLABEL_H
#define ELIDEDLABEL_H

#include <QFrame>

class ElidedLabel : public QFrame
{
    Q_OBJECT
  public:
    explicit ElidedLabel(QWidget *parent = nullptr, const QString &text = "");

    void setText(const QString &text);
    const QString &text() const;
    bool isElided() const;

    QFont getFont() const;
    void setFont(const QFont &newFont);

  signals:
    void elisionChanged(bool elided);

  protected:
    void paintEvent(QPaintEvent *event) override;

  private:
    bool elided;
    QString content;
    QFont font;
};
#endif // ELIDEDLABEL_H
