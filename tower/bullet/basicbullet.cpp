#include "basicbullet.h"
#include <QtMath>
#include <QVector2D>

basicBullet::basicBullet(baseEnemy* target, baseTower* tower) : baseBullet(target,tower)
{
    setPos(tower->x()+32,tower->y()+32); //TODO tower size

    QVector2D v2(QPointF(target->rect().x(),target->rect().y()));
    QVector2D v1(tower->pos());
    QVector2D dir = v2 - v1;
    dir.normalize();

    m_DirX = dir.x();
    m_DirY = dir.y();
    timer.start(16); //update position every 16ms

    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}


void basicBullet::advance(int phase){

}

void basicBullet::update(){
    setPos(x()+m_Speed*m_DirX, y()+m_Speed*m_DirY);

    //TODO this is very inefficient
    QList<QGraphicsItem *> list = collidingItems();

    if(list.size() > 1){
        foreach(QGraphicsItem * i , list)
        {
            baseEnemy* item = dynamic_cast<baseEnemy*>(i);
            if (item)
            {
                item->hit(1);
                deleteLater();
            }
        }
    }
}
