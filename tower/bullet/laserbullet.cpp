#include "laserbullet.h"
#include <QtMath>
#include <QVector2D>



laserBullet::laserBullet(baseEnemy* target, baseTower* tower) : baseBullet(target,tower)
{
    setPos(tower->x()+32,tower->y()+32); //TODO tower size


    //change color of the bullet
    //setParentItem(0);
    //setRect(-20.0,-20.0,5.0,5.0);
    setBrush(QBrush(QColor(0,0,255)));
    //setPen(QPen(Qt::NoPen));



    QVector2D v2(QPointF(target->rect().x(),target->rect().y()));
    QVector2D v1(tower->pos());
    QVector2D dir = v2 - v1;
    dir.normalize();

    m_DirX = dir.x();
    m_DirY = dir.y();
    timer.start(16); //update position every 16ms

    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}

void laserBullet::advance(int phase){

}
void laserBullet::update(){
    setPos(x()+m_Speed*m_DirX, y()+m_Speed*m_DirY);
    //delete bullet when its out of bounds
    if(x() > 800 || x() < 0 || y() > 640 || y() < 0){
        deleteLater();
    }else {
        //TODO this is very inefficient
        QList<QGraphicsItem *> list = collidingItems();

        if(list.size() > 1){
            foreach(QGraphicsItem * i , list)
            {
                baseEnemy* item = dynamic_cast<baseEnemy*>(i);
                if (item)
                {
                    item->hit(1);
                    deleteLater(); //Delete bullet
                    break;
                }
            }
        }
    }
}
