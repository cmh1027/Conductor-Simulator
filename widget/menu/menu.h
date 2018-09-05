#ifndef MENU_H
#define MENU_H
#include <QObject>

class MainWindow;

namespace Menu{
    class Menu : public QObject{
        Q_OBJECT

    public:
        explicit Menu(MainWindow*);
        virtual ~Menu() = default;
        virtual void setup() final;

    protected:
        MainWindow* parent;
        void clean();
        virtual void setupUi() = 0;
    };
}


#endif // MENU_H
