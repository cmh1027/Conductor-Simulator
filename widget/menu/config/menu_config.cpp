#include "conduct/tracker/tracker.h"
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QIntValidator>
#include <QLineEdit>
#include <QScrollBar>
#include <QFileDialog>
#include "menu_config.h"
#include "widget/mainwindow.h"
#include "widget/clickablelabel.h"
#include "ui_config.h"
#include "conduct/module/utility.h"
#include "conduct/command/command.h"

namespace Menu{
    Configuration::Configuration(MainWindow *parent) : Menu::Menu(parent),
        ui(new Ui::Configuration), tracker(new Tracker())
    {
    }

    Configuration::~Configuration(){
        delete ui;
        delete tracker;
    }

    void Configuration::setupUi(){
        ui->setupUi(parent);
        auto historySlider = parent->findChild<QSlider*>("historySlider");
        auto kernelSlider = parent->findChild<QSlider*>("kernelSlider");
        auto ratioSlider = parent->findChild<QSlider*>("ratioSlider");
        auto thresholdSlider = parent->findChild<QSlider*>("thresholdSlider");
        auto queueSizeSlider = parent->findChild<QSlider*>("queueSizeSlider");
        this->historyLabel = parent->findChild<QLabel*>("historyLabel");
        this->kernelLabel = parent->findChild<QLabel*>("kernelLabel");
        this->ratioLabel = parent->findChild<QLabel*>("ratioLabel");
        this->thresholdLabel = parent->findChild<QLabel*>("thresholdLabel");
        this->queueSizeLabel = parent->findChild<QLabel*>("queueSizeLabel");
        this->cameraLineEdit = parent->findChild<QLineEdit*>("cameraLineEdit");
        this->groupEnableButton = parent->findChild<QPushButton*>("groupEnableButton");
        historySlider->setValue(config.getHistory());
        kernelSlider->setValue(config.getKernel());
        thresholdSlider->setValue(config.getThreshold());
        ratioSlider->setValue(static_cast<int>(config.getRatio() * 100));
        queueSizeSlider->setValue(config.getQueueSize());
        historyLabel->setText(QString::number(historySlider->value()));
        kernelLabel->setText(QString::number(kernelSlider->value()));
        ratioLabel->setText(QString("%1%").arg(QString::number(ratioSlider->value())));
        thresholdLabel->setText(QString::number(thresholdSlider->value()));
        queueSizeLabel->setText(QString::number(queueSizeSlider->value()));
        cameraLineEdit->setValidator(new QIntValidator(0, 10));
        cameraLineEdit->setText(QString::number(config.camNumber));
        list = parent->findChild<QListWidget*>("listWidget");
        commandList = parent->findChild<QListWidget*>("commandListWidget");
        foreach(auto beat, Beats){
            commandList->addItem(beat);
        }
        foreach(auto beat, Commands){
            commandList->addItem(beat);
        }
        listScrollBar = list->verticalScrollBar();
        frameLabel = new ClickableLabel();
        frameLabel->setParent(parent->findChild<QWidget*>("frameWidget"));
        connect(parent->findChild<QPushButton*>("saveCameraButton"), &QPushButton::clicked, this, &Configuration::saveCamera);
        connect(tracker, QOverload<QString>::of(&Tracker::commandSignal), this, QOverload<const QString&>::of(&Configuration::addItem));
        connect(tracker, QOverload<QString, int>::of(&Tracker::commandSignal), this, QOverload<const QString&, int>::of(&Configuration::addItem));
        connect(parent->findChild<QPushButton*>("backButton"), &QPushButton::clicked, this, &Configuration::back);
        connect(parent->findChild<QPushButton*>("clearButton"), &QPushButton::clicked, this, &Configuration::clearList);
        connect(groupEnableButton, &QPushButton::clicked, this, &Configuration::toggleGroupEnable);
        connect(tracker, &Tracker::updatePictureSignal, this, &Configuration::updatePicture);
        connect(frameLabel, &ClickableLabel::clicked, this, &Configuration::setColor);
        connect(ratioSlider, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), this, &Configuration::setRatio);
        connect(thresholdSlider, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), this, &Configuration::setThreshold);
        connect(historySlider, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), this, &Configuration::setHistory);
        connect(kernelSlider, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), this, &Configuration::setKernel);
        connect(queueSizeSlider, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), this, &Configuration::setQueueSize);
        tracker->start();
    }

    void Configuration::addItem(const QString& text){
        foreach(auto item, commandList->selectedItems()){
            if(item->text() == text){
                this->list->addItem(text);
                listScrollBar->setValue(listScrollBar->maximum());
            }
        }
    }

    void Configuration::addItem(const QString& text, int distance){
        foreach(auto item, commandList->selectedItems()){
            if(item->text() == text){
                this->list->addItem(QString("%1 / Distance %2").arg(text).arg(distance));
                listScrollBar->setValue(listScrollBar->maximum());
            }
        }
    }

    void Configuration::saveCamera(){
        config.setCamera(cameraLineEdit->text().toInt());
    }

    void Configuration::back(){
        disconnect(tracker, nullptr, this, nullptr);
        tracker->stop();
        parent->setup_Main();
    }

    void Configuration::clearList(){
        this->list->clear();
    }

    void Configuration::updatePicture(Mat m1){
        frameLabel->setPixmap(mat2QPixmap(m1, QImage::Format_RGB888));
    }

    void Configuration::setColor(){
        auto pos = frameLabel->mapFromGlobal(QCursor::pos());
        QColor color = QColor::fromRgb(frameLabel->pixmap()->toImage().pixel(pos.x(), pos.y()));
        Scalar scalar = rgbTohsv(color.red(), color.green(), color.blue());
        config.setCurrentHsv(scalar);
        config.setCurrentRgb(color);
        this->parent->findChild<QLabel*>("colorLabel")->setText(config.refreshRgb());
    }

    void Configuration::setRatio(int value){
        config.setRatio(value / 100.0);
        ratioLabel->setText(QString("%1%").arg(QString::number(value)));
        config.refreshRgb();
    }

    void Configuration::setThreshold(int value){
        config.setThreshold(value);
        thresholdLabel->setText(QString("%1").arg(QString::number(value)));
    }

    void Configuration::setHistory(int value){
        config.setHistory(value);
        historyLabel->setText(QString::number(value));
    }

    void Configuration::setKernel(int value){
        config.setKernel(value);
        kernelLabel->setText(QString::number(value));
    }

    void Configuration::setQueueSize(int value){
        config.setQueueSize(value);
        queueSizeLabel->setText(QString::number(value));
    }

    void Configuration::toggleGroupEnable(){
        tracker->groupEnabled = !tracker->groupEnabled;
        if(tracker->groupEnabled)
            this->groupEnableButton->setText("Off");
        else
            this->groupEnableButton->setText("On");
    }

}
