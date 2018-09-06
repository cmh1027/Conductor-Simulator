#ifndef MENU_DIFFICULTY_H
#define MENU_DIFFICULTY_H

#include "../menu.h"
#include "conduct/conduct.h"

namespace Ui {
    class Difficulty;
}


namespace Menu{
    class SelectDifficulty : public Menu::Menu
    {
        Q_OBJECT

    public:
        explicit SelectDifficulty(MainWindow*);
        virtual ~SelectDifficulty() override;

    private:
        Ui::Difficulty *ui;
        void setupUi() override;

    public slots:
        void startGame(Difficulty);
        void back();
    };
}

#endif // MENU_DIFFICULTY_H
