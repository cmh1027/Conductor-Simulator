#include <QMenuBar>
#include "menu.h"
#include "widget/mainwindow.h"
namespace Menu{
    Menu::Menu(MainWindow* parent) : parent(parent){}

    void Menu::clean(){
        auto menu = parent->findChild<QMenuBar*>();
        if(menu != nullptr)
            delete menu;
        while(QWidget* widget = parent->findChild<QWidget*>("centralWidget")){
            delete widget;
        }
    }

    void Menu::setup(){
        this->clean();
        this->setupUi();
    }
}
