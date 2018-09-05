#include "clickablelabel.h"
ClickableLabel::ClickableLabel(QWidget* parent) : QLabel(){
    this->setParent(parent);
}

ClickableLabel::~ClickableLabel(){}

void ClickableLabel::mousePressEvent(QMouseEvent*){
    emit this->clicked();
}
