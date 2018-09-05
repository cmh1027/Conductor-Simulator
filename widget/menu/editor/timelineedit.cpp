#include "timelineedit.h"
#include "menu_editor.h"
#include <QString>
#include <QDoubleValidator>
#include <QFocusEvent>
TimeLineEdit::TimeLineEdit(Menu::Editor* parent, int num, const QString& str) : QLineEdit(str),
    parent(parent), row(num){
    auto validator = new QDoubleValidator(this);
    validator->setDecimals(1);
    validator->setBottom(0);
    this->setValidator(validator);
    connect(this, &QLineEdit::returnPressed, this, [=](){
        this->focusOutEvent(new QFocusEvent(QEvent::FocusOut));
    });
    this->setStyleSheet("border: none;");
}

void TimeLineEdit::focusInEvent(QFocusEvent* event){
    QLineEdit::focusInEvent(event);
    emit getFocus();
}

void TimeLineEdit::focusOutEvent(QFocusEvent* event){
    QLineEdit::focusOutEvent(event);
    if(this->text().isEmpty()){
        this->setText("0");
    }
    else{
        this->setText(QString::number(this->text().toDouble()));
    }
    emit this->parent->timeChanged(this);
}

void TimeLineEdit::setRow(int row){
    this->row = row;
}

int TimeLineEdit::getRow() const{
    return this->row;
}
