#include <QToolButton>
#include "menu_difficulty.h"
#include "ui_difficulty.h"
#include "widget/mainwindow.h"


namespace Menu{
    SelectDifficulty::SelectDifficulty(MainWindow *parent) : Menu::Menu(parent), ui(new Ui::Difficulty){}

    SelectDifficulty::~SelectDifficulty(){
        delete ui;
    }

    void SelectDifficulty::setupUi(){
        ui->setupUi(parent);
        connect(parent->findChild<QToolButton*>("easyButton"), &QToolButton::clicked, this, [=]{
           this->startGame(Difficulty::Easy);
        });
        connect(parent->findChild<QToolButton*>("normalButton"), &QToolButton::clicked, this, [=]{
           this->startGame(Difficulty::Normal);
        });
        connect(parent->findChild<QToolButton*>("hardButton"), &QToolButton::clicked, this, [=]{
           this->startGame(Difficulty::Hard);
        });
        connect(parent->findChild<QToolButton*>("backButton"), &QToolButton::clicked, this, &SelectDifficulty::back);
    }

    void SelectDifficulty::startGame(Difficulty difficulty){
        parent->setup_Conductor(difficulty);
    }

    void SelectDifficulty::back(){
        parent->setup_Main();
    }
}
