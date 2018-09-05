#ifndef TIMELINEEDIT_H
#define TIMELINEEDIT_H
#include <QLineEdit>

namespace Menu{
    class Editor;
}
class QString;
class TimeLineEdit : public QLineEdit{
    Q_OBJECT

public:
    TimeLineEdit(Menu::Editor*, int, const QString& = "");
    virtual ~TimeLineEdit() = default;
    void setRow(int);
    int getRow() const;

signals:
    void getFocus();
    void loseFocus();

private:
    Menu::Editor* parent;
    void focusInEvent(QFocusEvent*);
    void focusOutEvent(QFocusEvent*);
    int row;
};

#endif // TIMELINEEDIT_H
