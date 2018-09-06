#include "mainwindow.h"
Configuration config;

MainWindow::MainWindow() : mainMenu(this), difficultyMenu(this), conductorMenu(this), editorMenu(this), configMenu(this)
{
    mainMenu.setup();
}

void MainWindow::setup_Main(){
    mainMenu.setup();
}

void MainWindow::setup_Difficulty(){
    difficultyMenu.setup();
}

void MainWindow::setup_Conductor(Difficulty dif){
    conductorMenu.setup();
    conductorMenu.setDifficulty(dif);
}

void MainWindow::setup_Editor(){
    editorMenu.setup();
}

void MainWindow::setup_Config(){
    configMenu.setup();
}




