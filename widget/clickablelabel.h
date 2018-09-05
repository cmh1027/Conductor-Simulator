#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H
#include <QLabel>

class ClickableLabel : public QLabel{
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* = nullptr);
    virtual ~ClickableLabel();
    void mousePressEvent(QMouseEvent*);

signals:
    void clicked();

};
#endif // CLICKABLELABEL_H
