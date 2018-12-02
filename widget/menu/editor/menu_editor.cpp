#include <QSet>
#include <algorithm>
#include <QMessageBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QFileDialog>
#include <QString>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QtXml>
#include "menu_editor.h"
#include "ui_editor.h"
#include "widget/mainwindow.h"
#include "timelineedit.h"
#include "conduct/command/command.h"

namespace Menu{
    Editor::Editor(MainWindow *parent) : Menu::Menu(parent), ui(new Ui::Editor){}

    Editor::~Editor()
    {
        this->clearList();
        delete ui;
    }

    void Editor::setupUi(){
        ui->setupUi(parent);
        this->tableWidget = parent->findChild<QTableWidget*>();
        this->tableWidget->horizontalHeader()->setStretchLastSection(true);
        this->typeComboBox = parent->findChild<QComboBox*>("typeComboBox");
        this->actionComboBox = parent->findChild<QComboBox*>("actionComboBox");
        this->groupComboBox = parent->findChild<QComboBox*>("groupComboBox");
        this->musicTableWidget = parent->findChild<QTableWidget*>("musicTableWidget");
        this->musicTableWidget->setColumnWidth(MusicColumn::Main, 35);
        this->musicTableWidget->setColumnWidth(MusicColumn::Group, 70);
        this->musicTableWidget->horizontalHeader()->setStretchLastSection(true);
        this->typeChanged(0);
        connect(parent->findChild<QAction*>("actionLoad"), &QAction::triggered, this, &Editor::load);
        connect(parent->findChild<QAction*>("actionSave"), &QAction::triggered, this, &Editor::save);
        connect(parent->findChild<QAction*>("actionSaveAs"), &QAction::triggered, this, &Editor::saveAs);
        connect(parent->findChild<QPushButton*>("addButton"), &QPushButton::clicked, this, [=]{
            this->addCommandRow();
            this->refreshTable();
        });
        connect(parent->findChild<QPushButton*>("removeButton"), &QPushButton::clicked, this, &Editor::removeCommandRow);
        connect(this->typeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Editor::typeChanged);
        connect(this->actionComboBox, QOverload<int>::of(&QComboBox::activated), this, &Editor::setAction);
        connect(parent->findChild<QPushButton*>("addMusicButton"), &QPushButton::clicked, this, &Editor::addMusicFile);
        connect(parent->findChild<QPushButton*>("removeMusicButton"), &QPushButton::clicked, this, &Editor::removeMusicFile);
        connect(parent->findChild<QPushButton*>("toggleMainButton"), &QPushButton::clicked, this, &Editor::toggleMainMusic);
        connect(this->groupComboBox, QOverload<int>::of(&QComboBox::activated), this, &Editor::setMusicGroup);
        connect(parent->findChild<QPushButton*>("backButton"), &QPushButton::clicked, this, &Editor::back);
        for(int i = 1; i <= GroupCount; ++i){
            this->groupComboBox->addItem(QString::number(i));
        }
    }

    void Editor::clear(){
        this->tableWidget->setRowCount(0);
        this->musicTableWidget->setRowCount(0);
        this->clearList();
    }

    void Editor::clearList(){
        foreach(auto list, this->tableItemList){
            foreach(QString* ptr, list){
                delete ptr;
            }
        }
        this->tableItemList.clear();
    }

    void Editor::refreshTable(){
        int row = 0;
        std::sort(this->tableItemList.begin(), this->tableItemList.end(),
                  [](const QList<QString*>& list1, const QList<QString*>& list2) -> bool {
            return list1.at(CommandColumn::Time)->toDouble() < list2.at(CommandColumn::Time)->toDouble();
        });
        foreach(auto list, this->tableItemList){
            auto numberPtr = list.at(CommandColumn::Number);
            numberPtr->replace(0, numberPtr->length(), QString("#%1").arg(row));
            this->tableWidget->item(row, CommandColumn::Number)->setText(*list.at(CommandColumn::Number));
            this->tableWidget->cellWidget(row, CommandColumn::Time)->deleteLater();
            auto timeLineEdit = new TimeLineEdit(this, row);
            timeLineEdit->setText(*list.at(CommandColumn::Time));
            this->tableWidget->setCellWidget(row, CommandColumn::Time, timeLineEdit);
            this->tableWidget->item(row, CommandColumn::Action)->setText(*list.at(CommandColumn::Action));
            ++row;
        }
    }

    void Editor::typeChanged(int index){
        switch(index){
            case Type::Beat:
                this->beatItem();
                break;
            case Type::Dynamics:
                this->dynamicItem();
                break;
            case Type::Articulation:
                this->articulationItem();
                break;
            case Type::Group:
                this->groupItem();
                break;
        }
    }

    void Editor::beatItem(){
        this->actionComboBox->clear();
        foreach(auto item, Beats){
            this->actionComboBox->addItem(item);
        }
    }

    void Editor::dynamicItem(){
        this->actionComboBox->clear();
        foreach(auto item, Dynamics){
            this->actionComboBox->addItem(item);
        }
    }

    void Editor::articulationItem(){
        this->actionComboBox->clear();
        foreach(auto item, Commands){
            this->actionComboBox->addItem(item);
        }
    }

    void Editor::groupItem(){
        this->actionComboBox->clear();
        for(int i = 0; i <= GroupCount; ++i){
            this->actionComboBox->addItem(QString::number(i));
        }
    }

    void Editor::addCommandRow(){
        int row = this->tableWidget->rowCount();
        this->tableWidget->setRowCount(row+1);
        QTableWidgetItem* numberItem = new QTableWidgetItem();
        TimeLineEdit* timeItem = new TimeLineEdit(this, row);
        QTableWidgetItem* actionItem = new QTableWidgetItem();
        this->tableWidget->setItem(row, CommandColumn::Number, numberItem);
        this->tableWidget->setCellWidget(row, CommandColumn::Time, timeItem);
        this->tableWidget->setItem(row, CommandColumn::Action, actionItem);
        numberItem->setFlags(numberItem->flags() & ~Qt::ItemIsEditable);
        actionItem->setFlags(numberItem->flags() & ~Qt::ItemIsEditable);
        if(this->tableWidget->rowCount() == 1){
            this->appendItemList("", "0", "");
        }
        else{
            QString&& biggest = static_cast<QLineEdit*>(this->tableWidget->cellWidget(row - 1, CommandColumn::Time))->text();
            this->appendItemList("", QString::number(biggest.toDouble() + 1), "");
        }

    }

    void Editor::addCommandRow(const QString& number, const QString& time, const QString& action){
        int row = this->tableWidget->rowCount();
        this->tableWidget->setRowCount(row+1);
        QTableWidgetItem* numberItem = new QTableWidgetItem();
        TimeLineEdit* timeItem = new TimeLineEdit(this, row);
        QTableWidgetItem* actionItem = new QTableWidgetItem();
        this->tableWidget->setItem(row, CommandColumn::Number, numberItem);
        this->tableWidget->setCellWidget(row, CommandColumn::Time, timeItem);
        this->tableWidget->setItem(row, CommandColumn::Action, actionItem);
        timeItem->setStyleSheet("border: none;");
        numberItem->setFlags(numberItem->flags() & ~Qt::ItemIsEditable);
        actionItem->setFlags(numberItem->flags() & ~Qt::ItemIsEditable);
        this->appendItemList(number, time, action);
    }

    void Editor::appendItemList(const QString& number, const QString& time, const QString& action){
        QList<QString*> list;
        list.append(new QString(number));
        list.append(new QString(time));
        list.append(new QString(action));
        this->tableItemList.append(list);
    }

    void Editor::removeCommandRow(){
        while(this->tableWidget->selectedItems().length() != 0){
            int row = this->tableWidget->selectedItems().first()->row();
            this->tableWidget->removeRow(row);
            foreach(QString* ptr, this->tableItemList.at(row)){
                delete ptr;
            }
            this->tableItemList.removeAt(row);
            this->refreshTable();
        }
    }


    void Editor::addMusicFile(){
        QStringList &&fileNames = QFileDialog::getOpenFileNames(parent, "Select music", "./", "music files (*.wav *.mp3 *.mid)");
        bool exist;
        foreach(QString str, fileNames){
            exist = false;
            for(int row = 0; row < this->musicTableWidget->rowCount(); row++){
                if(this->musicTableWidget->item(row, MusicColumn::Path)->text() == str)
                    exist = true;
            }
            if(!exist){
                this->addMusicItem(str);
            }
        }
    }

    void Editor::addMusicItem(const QString& name, int group, bool isMain){
        int rowCount = this->musicTableWidget->rowCount();
        this->musicTableWidget->setRowCount(rowCount + 1);
        this->musicTableWidget->setItem(rowCount, MusicColumn::Main, new QTableWidgetItem(""));
        this->musicTableWidget->setItem(rowCount, MusicColumn::Group, new QTableWidgetItem(""));
        this->musicTableWidget->setItem(rowCount, MusicColumn::Path, new QTableWidgetItem(""));
        auto mainItem = this->musicTableWidget->item(rowCount, MusicColumn::Main);
        auto groupItem = this->musicTableWidget->item(rowCount, MusicColumn::Group);
        auto pathItem = this->musicTableWidget->item(rowCount, MusicColumn::Path);
        if(isMain)
            mainItem->setText("O");
        pathItem->setText(name);
        groupItem->setText(QString::number(group));
        mainItem->setFlags(mainItem->flags() & ~Qt::ItemIsEditable);
        groupItem->setFlags(groupItem->flags() & ~Qt::ItemIsEditable);
        pathItem->setFlags(pathItem->flags() & ~Qt::ItemIsEditable);
    }


    void Editor::removeMusicFile(){
        while(this->musicTableWidget->selectedItems().length() != 0){
            this->musicTableWidget->removeRow(this->musicTableWidget->selectedItems().first()->row());
        }
    }

    void Editor::toggleMainMusic(){
        auto list = this->musicTableWidget->selectedItems();
        QSet<int> selectedRows;
        foreach(auto item, list){
            int row = item->row();
            if(!selectedRows.contains(row))
                selectedRows.insert(row);
        }
        foreach(int row, selectedRows){
            auto item = this->musicTableWidget->item(row, MusicColumn::Main);
            if(item->text().isEmpty())
                item->setText("O");
            else{
                item->setText("");
            }
        }
    }

    void Editor::setMusicGroup(int){
        int row = this->musicTableWidget->currentRow();
        if(row != -1){
            QString&& text = this->groupComboBox->currentText();
            auto actionPtr = this->musicTableWidget->item(row, MusicColumn::Group);
            actionPtr->setText(text);
        }
    }

    void Editor::setAction(int){
        int row = this->tableWidget->currentRow();
        if(row != -1){
            QString&& text = this->actionComboBox->currentText();
            auto actionPtr = this->tableItemList.at(row).at(CommandColumn::Action);
            actionPtr->replace(0, actionPtr->length(), text);
            this->tableWidget->item(row, CommandColumn::Action)->setText(text);

        }
    }

    void Editor::load(){
        QString &&fileName = QFileDialog::getOpenFileName(parent, "Select xml", "./", "XML files (*.xml)");
        if(fileName.isEmpty())
            return;
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)){
            QMessageBox::critical(parent, "Error", "Failed to open the xml file");
            return;
        }
        this->clear();
        QDomDocument document;
        document.setContent(&file);
        file.close();
        if(!this->parseXML(document)){
            this->clear();
            QMessageBox::critical(parent, "Error", "Invalid XML file");
        }
        else
            this->filePath = fileName;
    }

    bool Editor::parseXML(const QDomDocument& document){
        QDomElement root = document.documentElement();
        QDomElement component = root.firstChild().toElement();
        if(root.toElement().isNull() || root.tagName() != "conduct")
            return false;
        while(!component.isNull()){
            if(component.tagName() == "commands"){
                if(!this->parseCommand(component))
                    return false;
            }
            else if(component.tagName() == "musics"){
                if(!this->parseMusic(component))
                    return false;
            }
            else if(component.tagName() == "volume"){
                this->parseVolume(component);
            }
            else if(component.tagName() == "tempo"){
                this->parseTempo(component);
            }
            else if(component.tagName() == "group"){
                this->parseGroup(component);
            }
            component = component.nextSibling().toElement();
        }
        return true;
    }

    bool Editor::parseCommand(const QDomElement& dom){
        QDomElement&& child = dom.firstChild().toElement();
        while(!child.isNull()){
            if(child.tagName() != "command" || !child.hasAttribute("time") || !child.hasAttribute("action"))
                return false;
            bool isNumber;
            QString&& time = child.attribute("time");
            time.toDouble(&isNumber);
            if(!isNumber)
                return false;
            QString&& action = child.attribute("action");
            this->addCommandRow("", time, action);
            child = child.nextSibling().toElement();
        }
        this->refreshTable();
        return true;
    }

    bool Editor::parseMusic(const QDomElement& dom){
        QDomElement&& child = dom.firstChild().toElement();
        while(!child.isNull()){
            if(child.text().isEmpty() || !child.hasAttribute("main"))
                return false;
            int group = child.attribute("group").toInt();
            if(!(1 <= group && group <= GroupCount))
                group = 1;
            this->addMusicItem(child.text(), group, child.attribute("main").toInt() != 0);
            child = child.nextSibling().toElement();
        }
        return true;
    }

    void Editor::parseVolume(const QDomElement& dom){
        if(dom.text() != "0")
            parent->findChild<QCheckBox*>("volumeCheckBox")->setChecked(true);
    }

    void Editor::parseTempo(const QDomElement& dom){
        if(dom.text() != "0")
            parent->findChild<QCheckBox*>("tempoCheckBox")->setChecked(true);
    }

    void Editor::parseGroup(const QDomElement& dom){
        if(dom.text() != "0")
            parent->findChild<QCheckBox*>("groupCheckBox")->setChecked(true);
    }

    bool Editor::isSaveable(){
        for(int row = 0; row < this->tableItemList.count(); ++row){
            for(int column = 0; column < this->tableItemList.at(row).count(); ++column){
                if(this->tableItemList.at(row).at(column)->isEmpty()){
                    QMessageBox::critical(parent, "Error", "Table item can not be empty\n" +
                                                         QString("Item (row : %1 / column : %2) is empty").arg(row).arg(column));
                    return false;
                }
            }
        }
        for(int row = 0; row < this->musicTableWidget->rowCount(); ++row){
            if(this->musicTableWidget->item(row, MusicColumn::Group)->text().isEmpty()){
                QMessageBox::critical(parent, "Error", "Specify a group number for music files");
                return false;
            }
        }
        if(this->tableItemList.count() == 0){
            QMessageBox::critical(parent, "Error", "Add at least one item");
            return false;
        }
        if(this->musicTableWidget->rowCount() == 0){
            QMessageBox::critical(parent, "Error", "Add at least one music file");
            return false;
        }

        return true;
    }

    void Editor::save(){
        if(this->filePath.isEmpty() || !QFile::exists(this->filePath))
            this->saveAs();
        else{
            if(!this->isSaveable())
                return;
            QDomDocument document;
            this->constructXML(document);
            this->saveXML(this->filePath, document);
        }
    }

    void Editor::saveAs(){
        if(!this->isSaveable())
            return;
        QString&& fileName = QFileDialog::getSaveFileName(parent, "Save file", "./", "XML (*.xml)");
        if(fileName.isEmpty())
            return;
        QDomDocument document;
        this->constructXML(document);
        if(this->saveXML(fileName, document))
            this->filePath = fileName;
    }

    bool Editor::saveXML(const QString& path, QDomDocument& document){
        QFile file(path);
        if(!file.open(QIODevice::WriteOnly)){
            QMessageBox::critical(parent, "Error", "Failed to save the file");
            return false;
        }
        else{
            QTextStream stream(&file);
            stream << document.toString();
            file.close();
            return true;
        }
    }

    void Editor::constructXML(QDomDocument& document){
        QDomElement &&root = document.createElement("conduct");
        QDomElement &&volume = document.createElement("volume");
        QDomElement &&tempo = document.createElement("tempo");
        QDomElement &&group = document.createElement("group");
        QDomElement &&commands = document.createElement("commands");
        QDomElement &&musics = document.createElement("musics");
        document.appendChild(root);
        root.appendChild(volume);
        root.appendChild(tempo);
        root.appendChild(group);
        root.appendChild(commands);
        root.appendChild(musics);
        volume.appendChild(document.createTextNode(QString::number(parent->findChild<QCheckBox*>("volumeCheckBox")->isChecked())));
        tempo.appendChild(document.createTextNode(QString::number(parent->findChild<QCheckBox*>("tempoCheckBox")->isChecked())));
        group.appendChild(document.createTextNode(QString::number(parent->findChild<QCheckBox*>("groupCheckBox")->isChecked())));
        foreach(auto list, this->tableItemList){
            QDomElement &&command = document.createElement("command");
            command.setAttribute("time", *list.at(CommandColumn::Time));
            command.setAttribute("action", *list.at(CommandColumn::Action));
            commands.appendChild(command);
        }
        for(int row = 0; row < this->musicTableWidget->rowCount(); ++row){
            QDomElement &&music = document.createElement("music");
            QDomText &&path = document.createTextNode(this->musicTableWidget->item(row, MusicColumn::Path)->text());
            if(!this->musicTableWidget->item(row, MusicColumn::Main)->text().isEmpty())
                music.setAttribute("main", "1");
            else
                music.setAttribute("main", "0");
            music.setAttribute("group", this->musicTableWidget->item(row, MusicColumn::Group)->text());
            music.appendChild(path);
            musics.appendChild(music);
        }
    }

    void Editor::timeChanged(TimeLineEdit* timeItem){
        auto timePtr = this->tableItemList.at(timeItem->getRow()).at(CommandColumn::Time);
        timePtr->replace(0, timePtr->length(), timeItem->text());
        this->refreshTable();
    }

    void Editor::back(){
        this->clearList();
        parent->setup_Main();
    }
}
