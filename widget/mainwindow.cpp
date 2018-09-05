#include "mainwindow.h"
Configuration config;

MainWindow::MainWindow() : mainMenu(this), conductorMenu(this), editorMenu(this), configMenu(this)
{
    mainMenu.setup();
}

void MainWindow::setup_Main(){
    mainMenu.setup();
}

void MainWindow::setup_Conductor(){
    conductorMenu.setup();
}

void MainWindow::setup_Editor(){
    editorMenu.setup();
}

void MainWindow::setup_Config(){
    configMenu.setup();
}




