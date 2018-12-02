#ifndef Editor_H
#define Editor_H

#include <QList>
#include "../menu.h"

class QTableWidget;
class QTableWidgetItem;
class QComboBox;
class QString;
class QTableWidget;
class QLineEdit;
class QDomDocument;
class QDomElement;
class TimeLineEdit;

namespace Ui {
    class Editor;
}

namespace Type{
    typedef enum {Beat, Dynamics, Articulation, Group} Type;
}
typedef enum {Number, Time, Action} CommandColumn;
typedef enum {Main, Group, Path} MusicColumn;

namespace Menu{
    class Editor : public Menu::Menu
    {
            Q_OBJECT

        public:
            explicit Editor(MainWindow*);
            virtual ~Editor() override;


        private:
            Ui::Editor *ui;
            QTableWidget* tableWidget;
            QComboBox* typeComboBox;
            QComboBox* actionComboBox;
            QComboBox* groupComboBox;
            QTableWidget* musicTableWidget;
            QList<QList<QString*>> tableItemList;
            QString filePath;
            void setupUi() override;
            void clear();
            void clearList();
            void refreshTable();
            void appendItemList(const QString&, const QString&, const QString&);
            void beatItem();
            void dynamicItem();
            void articulationItem();
            void groupItem();
            void addMusicItem(const QString&, int group = 1, bool = false);
            bool isSaveable();
            bool parseXML(const QDomDocument&);
            bool parseCommand(const QDomElement&);
            bool parseMusic(const QDomElement&);
            void parseVolume(const QDomElement&);
            void parseTempo(const QDomElement&);
            void parseGroup(const QDomElement&);
            bool saveXML(const QString&, QDomDocument&);
            void constructXML(QDomDocument&);

        public slots:
            void load();
            void timeChanged(TimeLineEdit*);
            void save();
            void saveAs();
            void typeChanged(int);
            void addCommandRow();
            void addCommandRow(const QString&, const QString&, const QString&);
            void removeCommandRow();
            void toggleMainMusic();
            void setMusicGroup(int);
            void setAction(int);
            void addMusicFile();
            void removeMusicFile();
            void back();
        };
}

#endif
