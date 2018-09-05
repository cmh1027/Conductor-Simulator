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
        auto cameraLineEdit = parent->findChild<QLineEdit*>("cameraLineEdit");
        cameraLineEdit->setValidator(new QIntValidator(0, 10));
        cameraLineEdit->setText(QString::number(config.camNumber));
        connect(parent->findChild<QPushButton*>("saveCameraButton"), &QPushButton::clicked, this, [=]{
            config.setCamera(cameraLineEdit->text().toInt());
        });
        connect(tracker, QOverload<QString>::of(&Tracker::commandSignal), this, [=](QString command){
           this->addItem(command);
        });
        connect(tracker, QOverload<QString, int>::of(&Tracker::commandSignal), this, [=](QString command, int distance){
           this->addItem(QString("%1 / Distance %2").arg(command).arg(distance));
        });
        connect(parent->findChild<QPushButton*>("backButton"), &QPushButton::clicked, this, [=]{
            disconnect(tracker, nullptr, this, nullptr);
            tracker->stop();
            parent->setup_Main();
        });
        connect(parent->findChild<QPushButton*>("clearButton"), &QPushButton::clicked, this, [=]{
            this->list->clear();
        });
        list = parent->findChild<QListWidget*>("listWidget");
        listScrollBar = list->verticalScrollBar();
        auto frameLabel = new ClickableLabel();
        frameLabel->setParent(parent->findChild<QWidget*>("frameWidget"));
        connect(tracker, &Tracker::updatePictureSignal, this, [=](Mat m1){
            frameLabel->setPixmap(mat2QPixmap(m1, QImage::Format_RGB888));
        });
        connect(frameLabel, &ClickableLabel::clicked, this, [=]{
            auto pos = frameLabel->mapFromGlobal(QCursor::pos());
            QColor color = QColor::fromRgb(frameLabel->pixmap()->toImage().pixel(pos.x(), pos.y()));
            Scalar scalar = rgbTohsv(color.red(), color.green(), color.blue());
            config.setCurrentHsv(scalar);
            config.setCurrentRgb(color);
            this->parent->findChild<QLabel*>("colorLabel")->setText(config.refreshRgb());
        });

        auto historySlider = parent->findChild<QSlider*>("historySlider");
        auto kernelSlider = parent->findChild<QSlider*>("kernelSlider");
        auto ratioSlider = parent->findChild<QSlider*>("ratioSlider");
        auto thresholdSlider = parent->findChild<QSlider*>("thresholdSlider");
        auto queueSizeSlider = parent->findChild<QSlider*>("queueSizeSlider");
        auto historyLabel = parent->findChild<QLabel*>("historyLabel");
        auto kernelLabel = parent->findChild<QLabel*>("kernelLabel");
        auto ratioLabel = parent->findChild<QLabel*>("ratioLabel");
        auto thresholdLabel = parent->findChild<QLabel*>("thresholdLabel");
        auto queueSizeLabel = parent->findChild<QLabel*>("queueSizeLabel");

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

        connect(ratioSlider, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), this,
                [=](int value){
            config.setRatio(value / 100.0);
            ratioLabel->setText(QString("%1%").arg(QString::number(value)));
            config.refreshRgb();
        });
        connect(thresholdSlider, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), this,
                [=](int value){
            config.setThreshold(value);
            thresholdLabel->setText(QString("%1").arg(QString::number(value)));
        });
        connect(historySlider, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), this,
                [=](int value){
            config.setHistory(value);
            historyLabel->setText(QString::number(value));
        });
        connect(kernelSlider, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), this,
                [=](int value){
            config.setKernel(value);
            kernelLabel->setText(QString::number(value));
        });
        connect(queueSizeSlider, static_cast<void(QSlider::*)(int)>(&QSlider::valueChanged), this,
                [=](int value){
            config.setQueueSize(value);
            queueSizeLabel->setText(QString::number(value));
        });
        tracker->start();
    }

    void Configuration::addItem(const QString& text){
        this->list->addItem(text);
        listScrollBar->setValue(listScrollBar->maximum());
    }

}
