#ifndef MYCOLORS_H
#define MYCOLORS_H

#include <QColor>

const QColor myLineColors[]
{
    QColor(255,0,0,255),
    QColor(0,255,0,255),
    QColor(0,120,255,255),
    QColor(255,42,212,255),

    QColor(127,255,170,255),
    QColor(85,212,255,255),
    QColor(255,255,0,255),
    QColor(0,255,255,255),

    QColor(255,255,255,255),
    QColor(250,200,0,255),
    QColor(204,160,255,255),
    QColor(140,200,255,255),

    QColor(120,200,200,255),
    QColor(182,182,182,255),
    QColor(255,170,210,255),
    QColor(200,150,70,255)
};

const QColor myBackColors[]
{
    QColor(230,255,240,255), // бледно-зеленый
    QColor(230,240,255,255), // бледно-голубой
    QColor(255,255,255,255), // белый
    QColor(240,240,240,255), // светлосерый
    QColor(145,145,145,255), // темносерый
    QColor(  0,  0,  0,255)  // черный
};

const Qt::PenStyle myPenStyles[]
{
    Qt::SolidLine,
    Qt::DashLine,
    Qt::DotLine,
    Qt::DashDotLine,
    Qt::DashDotDotLine
};
#endif // MYCOLORS_H
