#include <QPainter>
#include <algorithm>
#include <iostream>
#include "scoremaker.h"
#include "conduct/command/command.h"
#include "conduct/module/synctimer.h"


MusicSheet::MusicSheet(const QQueue<SyncTimer*>* commands) :
    QWidget(), commands(commands){}

void MusicSheet::paintEvent(QPaintEvent*){
    QPainter painter;
    painter.begin(this);
    painter.setPen(Qt::black);
    painter.setFont(QFont("MaestroTimes", 10));
    this->drawBackground(painter);
    for(auto it = commands->begin(); it != commands->end(); ++it){
        if(Beats.contains((*it)->command))
            this->drawBeat(painter, (*it)->command, (*it)->getTime() / (*it)->getTickInterval());
        else if(Dynamics.contains((*it)->command))
            this->drawDynamic(painter, (*it)->command, (*it)->getTime() / (*it)->getTickInterval());
        else if(Commands.contains((*it)->command))
            this->drawCommand(painter, (*it)->command, (*it)->getTime() / (*it)->getTickInterval());
        else // group
            this->drawCommand(painter, (*it)->command, (*it)->getTime() / (*it)->getTickInterval());
    }

    painter.end();
}

void MusicSheet::drawBackground(QPainter& painter){
    painter.drawLine(0, height()/2, width()-1, height()/2);
    painter.drawLine(0, height()/4, 0, height()*3/4);
    painter.drawLine(width()-1, height()/4, width()-1, height()*3/4);
}

void MusicSheet::drawCommand(QPainter& painter, const QString command, int x){
    painter.drawText(x * 3, height()/4, Marks[command]);
}

void MusicSheet::drawDynamic(QPainter& painter, const QString command, int x){
    painter.drawText(x * 3, height()*3/4, Marks[command]);
}

void MusicSheet::drawBeat(QPainter& painter, const QString command, int x){
    painter.drawText(x * 3, height()/4, Marks[command]);
    painter.drawLine(x * 3, height()*3/8, x * 3, height()*5/8);
}

void MusicSheet::drawGroup(QPainter& painter, const QString command, int x){
     painter.drawText(x * 3, height()*3/4, command);
}


QWidget* ScoreMaker::makeScore(const QQueue<SyncTimer*>* timers, int difInterval, int lastInterval, int height){
    auto tail = std::max_element(timers->begin(), timers->end(), [](const SyncTimer* timer1, const SyncTimer* timer2){
        return timer1->getTime() < timer2->getTime();
    });
    MusicSheet* score = new MusicSheet(timers);
    score->setFixedWidth(((*tail)->getTime() + difInterval + lastInterval) * 3 / (*tail)->getTickInterval());
    score->setFixedHeight(height);
    return score;
}
