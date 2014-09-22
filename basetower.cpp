#include "basetower.h"

baseEnemy::baseEnemy(qreal x, qreal y, QGraphicsItem * parent) :QGraphicsEllipseItem(x,y,32,32, parent)
{
    setPos(x,y);
    this->setBrush(QBrush(QColor(255,0,0)));
    this->setVisible(true);
}

void baseEnemy::setCenterRect(QPointF position){
    //Set device to location
    QRectF rect = this->rect();
    rect.moveCenter(position);
    setRect(rect);
}
