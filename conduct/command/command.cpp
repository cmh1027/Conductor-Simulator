#include "command.h"
namespace Command{
    const QString Vertical = "Vertical";
    const QString Horizontal = "Horizontal";
    const QString Whip = "Whip";
    const QString ff = "ff";
    const QString f = "f";
    const QString p = "p";
    const QString pp = "pp";
    const QString Accent = "Accent";
    const QString ShortAccent = "Short Accent";
    const QString ReverseAccent = "Reverse Accent";
    const QString Staccato = "Staccato";
}

const QSet<QString> Dynamics = {Command::ff, Command::f, Command::p, Command::pp};
const QSet<QString> Beats = {Command::Vertical, Command::Horizontal};
const QSet<QString> Commands = {Command::Whip, Command::Accent, Command::ShortAccent,
                                Command::ReverseAccent, Command::Staccato};

const QMap<QString, QString> Marks = {{Command::Vertical, "V"}, {Command::Horizontal, "H"},
                                      {Command::Whip, "W"}, {Command::ff, "ff"}, {Command::f, "f"},
                                      {Command::p, "p"}, {Command::pp, "pp"}, {Command::Accent, ">"},
                                      {Command::ShortAccent, "≥"}, {Command::ReverseAccent, "｜"},
                                      {Command::Staccato, "•"}};
