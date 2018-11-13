#ifndef MENU_CONFIG_H
#define MENU_CONFIG_H
#include "../menu.h"
#include "conduct/config/config.h"

extern Configuration config;

namespace Ui {
    class Configuration;
}

class Tracker;
class QListWidget;
class QScrollBar;
class QSlider;
class QLabel;
class QLineEdit;
class ClickableLabel;

namespace Menu{
    class Configuration : public Menu::Menu{
        Q_OBJECT

    public:
        explicit Configuration(MainWindow*);
        virtual ~Configuration() override;

    private:
        void setupUi() override;
        Ui::Configuration* ui;
        QListWidget* list;
        QListWidget* commandList;
        QScrollBar* listScrollBar;
        Tracker* tracker;
        QSlider* historySlider;
        QSlider* kernelSlider;
        QSlider* ratioSlider;
        QSlider* thresholdSlider;
        QSlider* queueSizeSlider;
        QLabel* historyLabel;
        QLabel* kernelLabel;
        QLabel* ratioLabel;
        QLabel* thresholdLabel;
        QLabel* queueSizeLabel;
        QLineEdit* cameraLineEdit;
        ClickableLabel* frameLabel;

    public slots:
        void addItem(const QString&);
        void addItem(const QString&, int);
        void saveCamera();
        void back();
        void clearList();
        void updatePicture(Mat);
        void setColor();
        void setRatio(int);
        void setThreshold(int);
        void setHistory(int);
        void setKernel(int);
        void setQueueSize(int);

    };
}
#endif // MENU_CONFIG_H
