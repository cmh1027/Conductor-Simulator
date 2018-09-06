#ifndef MENU_CONDUCTOR_H
#define MENU_CONDUCTOR_H

#include "../menu.h"
#include "conduct/conduct.h"
#include <QColor>
#include <opencv2/opencv.hpp>

namespace Ui {
    class Conductor;
}
class QListWidget;
class ConductSimulator;
class Tracker;
class QWidget;
class QScrollBar;
class QLabel;
class QTimer;

using namespace cv;

extern const int ScoreInterval;
extern const int DisplayTime;

namespace Menu{
    class Conductor : public Menu::Menu
    {
        Q_OBJECT

    public:
        explicit Conductor(MainWindow*);
        virtual ~Conductor() override;
        void setDifficulty(Difficulty);

    private:
        void setupUi() override;
        Tracker* tracker;
        Ui::Conductor *ui;
        QTimer* commandTimer;
        QTimer* dynamicTimer;
        ConductSimulator* simulator;
        QListWidget* list;
        QScrollBar* listScrollBar;
        QWidget* sheet;
        QLabel* scoreLabel;
        QLabel* energyLabel;
        QLabel* commandLabel;
        QLabel* dynamicLabel;
        QLabel* frameLabel;
        void rewindSheet();
        void setCommandLabel(const QString&, const QString&);
        void setDynamicLabel(const QString&, const QString&);

    public slots:
        void addItem(const QString&);
        void clear();
        void command(Precision);
        void dynamic(Precision);
        void selectFile();
        void back();
        void setScore(int);
        void setEnergy(int);
        void gameover();
        void tick();
        void setSheet(const QQueue<SyncTimer*>*, int, int);
        void init();
        void end();
        void start();
        void pause();
        void updatePicture(const Mat&);
    };
}

#endif // MENU_CONDUCTOR_H
