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
        QScrollBar* listScrollBar;
        Tracker* tracker;

    public slots:
        void addItem(const QString&);

    };
}
#endif // MENU_CONFIG_H
