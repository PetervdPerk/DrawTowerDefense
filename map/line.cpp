#include "map/line.h"

Line::Line(QObject *parent) :
    QObject(parent)
{
    path = new QPainterPath();
    path->moveTo(100,100);
    path->quadTo(100,600,400,400);
}
