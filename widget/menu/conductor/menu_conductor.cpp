#include "menu_conductor.h"
#include <QTimer>
#include <QFileDialog>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QQueue>
#include <QScrollBar>
#include "widget/mainwindow.h"
#include "ui_conductor.h"
#include "conduct/tracker/tracker.h"
#include "conduct/module/utility.h"
#include "conduct/module/synctimer.h"
#include "scoremaker.h"

const int ScoreInterval = 90;
const int DisplayTime = 500;

namespace Menu{
    Conductor::Conductor(MainWindow *parent) : Menu::Menu(parent), tracker(new Tracker()),
        ui(new Ui::Conductor), commandTimer(new QTimer()), dynamicTimer(new QTimer()),
        simulator(new ConductSimulator()), sheet(nullptr)
    {
        simulator->applyTracker(tracker);
        connect(commandTimer, &QTimer::timeout, this, [=]{
            this->commandLabel->setText("");
        });
        connect(dynamicTimer, &QTimer::timeout, this, [=]{
            this->dynamicLabel->setText("");
        });
    }

    Conductor::~Conductor(){
        delete ui;
        delete simulator;
        delete tracker;
        delete commandTimer;
        delete dynamicTimer;
    }

    void Conductor::setupUi(){
        ui->setupUi(parent);
        list = parent->findChild<QListWidget*>("listWidget");
        listScrollBar = list->verticalScrollBar();
        scoreLabel = parent->findChild<QLabel*>("scoreLabel");
        energyLabel = parent->findChild<QLabel*>("energyLabel");
        commandLabel = parent->findChild<QLabel*>("commandLabel");
        dynamicLabel = parent->findChild<QLabel*>("dynamicLabel");
        frameLabel = parent->findChild<QLabel*>("frameLabel");
        sheet = nullptr;
        connect(parent->findChild<QPushButton*>("openButton"), &QPushButton::clicked, this, &Conductor::selectFile);
        connect(parent->findChild<QPushButton*>("backButton"), &QPushButton::clicked, this, &Conductor::back);
        connect(simulator, &ConductSimulator::scoreChangedSignal, this, &Conductor::setScore);
        connect(simulator, &ConductSimulator::energyChangedSignal, this, &Conductor::setEnergy);
        connect(simulator, &ConductSimulator::gameoverSignal, this, &Conductor::gameover);
        connect(simulator, &ConductSimulator::infoSignal, this, &Conductor::addItem);
        connect(simulator, &ConductSimulator::tickSignal, this, &Conductor::tick);
        connect(simulator, &ConductSimulator::initSignal, this, &Conductor::init);
        connect(simulator, &ConductSimulator::endSignal, this, &Conductor::end);
        connect(simulator, &ConductSimulator::makeSheetSignal, this, &Conductor::setSheet);
        connect(simulator, &ConductSimulator::commandSignal, this, &Conductor::command);
        connect(simulator, &ConductSimulator::dynamicSignal, this, &Conductor::dynamic);
        connect(parent->findChild<QPushButton*>("startButton"), &QPushButton::clicked, this, &Conductor::start);
        connect(parent->findChild<QPushButton*>("stopButton"), &QPushButton::clicked, this, &Conductor::pause);
        connect(tracker, &Tracker::updatePictureSignal, this, &Conductor::updatePicture);
        simulator->start();
    }


    void Conductor::addItem(const QString& text){
        list->addItem(text);
        listScrollBar->setValue(listScrollBar->maximum());
    }

    void Conductor::clear(){
        list->clear();
    }

    void Conductor::setSheet(const QQueue<SyncTimer*>* timers, int difInterval, int lastInterval){
        if(sheet != nullptr)
            delete sheet;
        this->sheet = ScoreMaker::makeScore(timers, difInterval, lastInterval, parent->findChild<QWidget*>("sheetWidget")->height());
        sheet->setParent(parent->findChild<QWidget*>("sheetWidget"));
        sheet->move(ScoreInterval, 0);
        sheet->show();
        sheet->lower();
    }

    void Conductor::rewindSheet(){
        this->sheet->move(ScoreInterval, 0);
    }

    void Conductor::setCommandLabel(const QString& text, const QString& color){
        this->commandLabel->setStyleSheet(QString("border: none; color: %1; background: transparent;").arg(color));
        this->commandLabel->setText(text);
        this->commandTimer->start(DisplayTime);
    }

    void Conductor::setDynamicLabel(const QString& text, const QString& color){
        this->dynamicLabel->setStyleSheet(QString("border: none; color: %1; background: transparent;").arg(color));
        this->dynamicLabel->setText(text);
        this->dynamicTimer->start(DisplayTime);
    }

    void Conductor::command(Precision precision){
        switch(precision){
            case Perfect:
            this->setCommandLabel("Perfect", "green");
            break;
        case Excellent:
            this->setCommandLabel("Excellent", "orange");
            break;
        case Good:
            this->setCommandLabel("Good", "yellow");
            break;
        case Bad:
            this->setCommandLabel("Bad", "rgb(210, 155, 210)");
            break;
        case Fail:
            this->setCommandLabel("Fail", "rgb(153, 68, 153)");
            break;
        }
    }

    void Conductor::dynamic(Precision precision){
        switch(precision){
            case Perfect:
            this->setDynamicLabel("Perfect", "green");
            break;
        case Excellent:
            this->setDynamicLabel("Excellent", "orange");
            break;
        case Good:
            this->setDynamicLabel("Good", "yellow");
            break;
        case Bad:
            this->setDynamicLabel("Bad", "rgb(210, 155, 210)");
            break;
        case Fail:
            this->setDynamicLabel("Fail", "rgb(153, 68, 153)");
            break;
        }
    }

    void Conductor::setDifficulty(Difficulty dif){
        this->simulator->setDifficulty(dif);
    }

    void Conductor::selectFile(){
        QString &&fileName = QFileDialog::getOpenFileName(parent, "Select xml", "./", "XML files (*.xml)");
        simulator->loadXML(fileName);
    }

    void Conductor::back(){
        disconnect(tracker, nullptr, this, nullptr);
        disconnect(simulator, nullptr, this, nullptr);
        this->commandTimer->stop();
        this->dynamicTimer->stop();
        simulator->clear();
        parent->setup_Main();
    }

    void Conductor::setScore(int score){
        scoreLabel->setText(QString::number(score));
    }

    void Conductor::setEnergy(int energy){
        energyLabel->resize(energy * 3, energyLabel->height());
    }

    void Conductor::gameover(){
        this->addItem("Game over");
    }

    void Conductor::tick(){
        if(this->sheet != nullptr)
            this->sheet->move(this->sheet->x() - 3, this->sheet->y());
    }

    void Conductor::init(){
        this->list->clear();
    }

    void Conductor::end(){
        this->addItem("End");
    }

    void Conductor::start(){
        if(simulator->isPausing()){
            simulator->resume();
        }
        else{
            this->rewindSheet();
            simulator->gameStart();
        }
    }

    void Conductor::pause(){
        simulator->pause();
    }

    void Conductor::updatePicture(const Mat& m1){
        this->frameLabel->setPixmap(mat2QPixmap(m1, QImage::Format_RGB888));
    }


}
