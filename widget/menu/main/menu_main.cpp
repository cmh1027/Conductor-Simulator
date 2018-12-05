#include <QPushButton>
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
        connect(parent->findChild<QToolButton*>("playButton"), &QToolButton::clicked, parent, &MainWindow::setup_Difficulty);
        connect(parent->findChild<QToolButton*>("editorButton"), &QToolButton::clicked, parent, &MainWindow::setup_Editor);
        connect(parent->findChild<QToolButton*>("configButton"), &QToolButton::clicked, parent, &MainWindow::setup_Config);
    }

}
