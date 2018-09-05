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
    typedef enum {Beat, Dynamics, Articulation} Type;
}
typedef enum {Number, Time, Action} CommandColumn;
typedef enum {Main, Path} MusicColumn;

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
            QTableWidget* musicTableWidget;
            QList<QList<QString*>> tableItemList;
            QString filePath;
            void setupUi() override;
            void clear();
            void clearList();
            void refreshTable();
            void typeChanged(int);
            void addCommandRow();
            void addCommandRow(const QString&, const QString&, const QString&);
            void removeCommandRow();
            void appendItemList(const QString&, const QString&, const QString&);
            void beatItem();
            void dynamicItem();
            void articulationItem();
            void setAction(int, const QString&);
            void addMusicFile();
            void addMusicItem(const QString&, bool = false);
            void removeMusicFile();
            void toggleMainMusic();
            void load();
            bool isSaveable();
            void save();
            void saveAs();
            bool parseXML(const QDomDocument&);
            bool parseCommand(const QDomElement&);
            bool parseMusic(const QDomElement&);
            bool parseVolume(const QDomElement&);
            bool parseTempo(const QDomElement&);
            bool saveXML(const QString&, QDomDocument&);
            void constructXML(QDomDocument&);

        public slots:
            void timeChanged(TimeLineEdit*);
        };
}

#endif
