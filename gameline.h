#ifndef GAMELINE_H
#define GAMELINE_H

#include <QGraphicsPolygonItem>

class GameLine : public QGraphicsPolygonItem
{
public:
    GameLine(const QPolygonF & polygon, QGraphicsItem * parent = 0);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    int fract = 0;
};

#endif // GAMELINE_H
