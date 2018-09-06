#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QIntValidator>
#include <QLineEdit>
#include <QScrollBar>
#include "menu_config.h"
#include "widget/mainwindow.h"
#include "widget/clickablelabel.h"
#include "ui_config.h"
#include "conduct/tracker/tracker.h"
#include "conduct/module/utility.h"

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
        historySlider = parent->findChild<QSlider*>("historySlider");
        kernelSlider = parent->findChild<QSlider*>("kernelSlider");
        ratioSlider = parent->findChild<QSlider*>("ratioSlider");
        thresholdSlider = parent->findChild<QSlider*>("thresholdSlider");
        queueSizeSlider = parent->findChild<QSlider*>("queueSizeSlider");
        historyLabel = parent->findChild<QLabel*>("historyLabel");
        kernelLabel = parent->findChild<QLabel*>("kernelLabel");
        ratioLabel = parent->findChild<QLabel*>("ratioLabel");
        thresholdLabel = parent->findChild<QLabel*>("thresholdLabel");
        queueSizeLabel = parent->findChild<QLabel*>("queueSizeLabel");
        cameraLineEdit = parent->findChild<QLineEdit*>("cameraLineEdit");
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
        listScrollBar = list->verticalScrollBar();
        frameLabel = new ClickableLabel();
        frameLabel->setParent(parent->findChild<QWidget*>("frameWidget"));
        connect(parent->findChild<QPushButton*>("saveCameraButton"), &QPushButton::clicked, this, &Configuration::saveCamera);
        connect(tracker, QOverload<QString>::of(&Tracker::commandSignal), this, QOverload<const QString&>::of(&Configuration::addItem));
        connect(tracker, QOverload<QString, int>::of(&Tracker::commandSignal), this, QOverload<const QString&, int>::of(&Configuration::addItem));
        connect(parent->findChild<QPushButton*>("backButton"), &QPushButton::clicked, this, &Configuration::back);
        connect(parent->findChild<QPushButton*>("clearButton"), &QPushButton::clicked, this, &Configuration::clearList);
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
        this->list->addItem(text);
        listScrollBar->setValue(listScrollBar->maximum());
    }

    void Configuration::addItem(const QString& text, int distance){
        this->list->addItem(QString("%1 / Distance %2").arg(text).arg(distance));
        listScrollBar->setValue(listScrollBar->maximum());
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

}
