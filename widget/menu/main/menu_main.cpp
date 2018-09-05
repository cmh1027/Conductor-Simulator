#include <QPushButton>
#include <iostream>
#include "menu_main.h"
#include "ui_main.h"
#include "widget/mainwindow.h"
namespace Menu{

    Main::Main(MainWindow* parent) : Menu(parent), ui(new Ui::Main) {}

    Main::~Main(){
        delete ui;
    }

    void Main::setupUi(){
        ui->setupUi(parent);
        parent->resize(305, parent->height());
        connect(parent->findChild<QToolButton*>("playButton"), &QPushButton::clicked, parent, &MainWindow::setup_Conductor);
        connect(parent->findChild<QToolButton*>("editorButton"), &QPushButton::clicked, parent, &MainWindow::setup_Editor);
        connect(parent->findChild<QToolButton*>("configButton"), &QPushButton::clicked, parent, &MainWindow::setup_Config);
    }

}
