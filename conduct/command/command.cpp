#include "command.h"
namespace Command{
    const QString Vertical = "Vertical";
    const QString Horizontal = "Horizontal";
    const QString VerticalSwing = "Vertical Swing";
    const QString HorizontalSwing = "Horizontal Swing";
    const QString Whip = "Whip";
    const QString DiagonalWhip = "Diagonal Whip";
    const QString ff = "ff";
    const QString f = "f";
    const QString mf = "mf";
    const QString mp = "mp";
    const QString p = "p";
    const QString pp = "pp";
    const QString Accent = "Accent";
    const QString ShortAccent = "Short Accent";
    const QString ReverseAccent = "Reverse Accent";
    const QString Staccato = "Staccato";

}

const QSet<QString> Dynamics = {Command::ff, Command::f, Command::mf, Command::mp, Command::p, Command::pp};
const QSet<QString> Beats = {Command::Vertical, Command::Horizontal};
const QSet<QString> Commands = {Command::Whip, Command::DiagonalWhip, Command::Accent, Command::ShortAccent,
                                Command::ReverseAccent, Command::Staccato, Command::VerticalSwing, Command::HorizontalSwing};

const QMap<QString, Dynamic> DynamicMap = {{Command::ff, Dynamic::ff}, {Command::f, Dynamic::f}, {Command::mf, Dynamic::mf},
                                          {Command::mp, Dynamic::mp}, {Command::p, Dynamic::p}, {Command::pp, Dynamic::pp}};

const QMap<QString, QString> Marks = {{Command::Vertical, "V"}, {Command::Horizontal, "H"},
                                      {Command::VerticalSwing, "v"}, {Command::HorizontalSwing, "h"},
                                      {Command::Whip, "─"}, {Command::DiagonalWhip, "/"},
                                      {Command::ff, "ff"}, {Command::f, "f"}, {Command::mf, "mf"}, {Command::mp, "mp"},
                                      {Command::p, "p"}, {Command::pp, "pp"}, {Command::Accent, ">"},
                                      {Command::ShortAccent, "≥"}, {Command::ReverseAccent, "｜"},
                                      {Command::Staccato, "•"}};

const int GroupCount = 4;
