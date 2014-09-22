#include "bullet.h"

Bullet::Bullet(baseEnemy* target, baseTower* tower = 0)
    : m_Speed(1.0)
    , QGraphicsEllipseItem(tower)
{
    this->target = target;
    setParentItem(0);
    setRect(-20.0,-20.0,5.0,5.0);
    setBrush(QBrush(QColor(255,0,0)));
    setPen(QPen(Qt::NoPen));

    animation = new QPropertyAnimation(this,"pos");

    animation->setDuration(1000);
    QRectF towerPos = tower->rect();
    towerPos.setTopLeft(tower->pos());
    animation->setStartValue(tower->pos());
    animation->setEndValue(target->rect().center());
    animation->start();

    QObject::connect(animation, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void Bullet::setCenterRect(QPointF position){
    //Set device to location
    QRectF rect = this->rect();

    rect.moveCenter(position);

    //qDebug()  <<  "Tick" << position.x() << " y:" << position.y();
    setRect(rect);
}

void Bullet::advance(int phase){
    if(target != NULL){
        //animation->setEndValue(target->rect().center());
        QList<QGraphicsItem *> list = collidingItems();

        if(list.size() > 1){
            foreach(QGraphicsItem * i , list)
            {
                baseEnemy* item = dynamic_cast<baseEnemy*>(i);
                if (item)
                {
                    target->hit(1);
                    deleteLater();
                }
            }
        }
    }
}
