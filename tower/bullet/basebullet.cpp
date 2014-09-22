#include "basebullet.h"

baseBullet::baseBullet(baseEnemy* target, baseTower* tower = 0)
    : m_Speed(1.0)
    , QGraphicsEllipseItem(tower)
{
    this->target = target;
    setParentItem(0);
    setRect(-20.0,-20.0,5.0,5.0);
    setBrush(QBrush(QColor(255,0,0)));
    setPen(QPen(Qt::NoPen));
}

void baseBullet::setCenterRect(QPointF position){
    //Set device to location
    QRectF rect = this->rect();

    rect.moveCenter(position);

    //qDebug()  <<  "Tick" << position.x() << " y:" << position.y();
    setRect(rect);
}

void baseBullet::advance(int phase){

}
