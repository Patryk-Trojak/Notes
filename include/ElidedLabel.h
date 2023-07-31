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

  protected:
    void paintEvent(QPaintEvent *event) override;

  signals:
    void elisionChanged(bool elided);

  private:
    bool elided;
    QString content;
};
#endif // ELIDEDLABEL_H
