#include "gameline.h"
#include <QPainter>

GameLine::GameLine(const QPolygonF & polygon, QGraphicsItem *parent) :
    QGraphicsPolygonItem(polygon,parent)
{
}



void GameLine::paint(QPainter *painter,
                     const QStyleOptionGraphicsItem *option,
                     QWidget *widget) {
    // fill part of polygon
    painter->setPen(Qt::NoPen);
    painter->setBrush(brush());
    QRectF rect = boundingRect();
    rect.setTop(rect.bottom() - fract * rect.height());
    painter->drawPolygon(polygon().intersected(rect), fillRule());
    // draw polygon outline
    painter->setPen(pen());
    painter->setBrush(QBrush(QColor("Black")));
    painter->drawPolygon(polygon(), fillRule());
}
