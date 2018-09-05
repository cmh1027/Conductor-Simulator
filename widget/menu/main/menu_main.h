#ifndef MENU_MAIN_H
#define MENU_MAIN_H
#include "../menu.h"

namespace Ui {
    class Main;
}

namespace Menu{
    class Main : public Menu::Menu{
        Q_OBJECT
    public:
        explicit Main(MainWindow*);
        virtual ~Main() override;

    private:
        Ui::Main* ui;
        void setupUi() override;
    };
}

#endif // MENU_MAIN_H
