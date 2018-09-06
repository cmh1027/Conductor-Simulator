#ifndef COMMAND_H
#define COMMAND_H
#include <QString>
#include <QSet>
#include <QMap>
typedef enum {pp, p, mp, mf, f, ff, None} Dynamic;
extern const QSet<QString> Dynamics;
extern const QSet<QString> Beats;
extern const QSet<QString> Commands;
extern const QMap<QString, Dynamic> DynamicMap;
extern const QMap<QString, QString> Marks;

namespace Command{
    extern const QString Vertical;
    extern const QString Horizontal;
    extern const QString Whip;
    extern const QString DiagonalWhip;
    extern const QString ff;
    extern const QString f;
    extern const QString mf;
    extern const QString mp;
    extern const QString p;
    extern const QString pp;
    extern const QString Accent;
    extern const QString ShortAccent;
    extern const QString ReverseAccent;
    extern const QString Staccato;
}
#endif // COMMAND_H
