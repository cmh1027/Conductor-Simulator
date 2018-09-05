#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "menu/main/menu_main.h"
#include "menu/conductor/menu_conductor.h"
#include "menu/config/menu_config.h"
#include "menu/editor/menu_editor.h"
#include "conduct/config/config.h"

extern Configuration config;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    virtual ~MainWindow() = default;

private:
    Menu::Main mainMenu;
    Menu::Conductor conductorMenu;
    Menu::Editor editorMenu;
    Menu::Configuration configMenu;

public slots:
    void setup_Main();
    void setup_Conductor();
    void setup_Editor();
    void setup_Config();
};

#endif // MAINWINDOW_H
