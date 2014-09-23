#include "basicbullet.h"
#include <QtMath>
#include <QVector2D>

basicBullet::basicBullet(baseEnemy* target, baseTower* tower) : baseBullet(target,tower)
{

/*
    qreal dirx = target->rect().x();
    qreal diry = target->rect().y();

    qreal length = sqrt(dirx*dirx+diry*diry);
    if (length!=0)
    {
        // normalized direction vector
        qreal invLength= 1.0/length;
        dirx *= invLength;
        diry *= invLength;

        // creating an angle perturbation of +/- 3°
       // qreal alphaPerturbation = static_cast<qreal>(qrand()%6-3) * M_PI / 180.0;
       // qreal xPerturbation = cos(alphaPerturbation);
       // qreal yPerturbation = sin(alphaPerturbation);
        // cos(a+b)=...
        dirx = dirx - diry;
                // sin(a+b)=...
                diry = diry + dirx;

        m_DirX = dirx;
        m_DirY = diry;
    }



QMatrix
*/

    /*

    v1 = ( Player.x, Player.y );
    v2 = ( Mouse.x, Mouse.y );
    dir = v2 - v1;
    dir.normalize();
    arrow.xy += dir * speed;*/

    setPos(tower->x()+32,tower->y()+32);

    QVector2D v2(QPointF(target->rect().x(),target->rect().y()));
    QVector2D v1(tower->pos());
    QVector2D dir = v2 - v1;
    dir.normalize();

    m_DirX = dir.x();
    m_DirY = dir.y();



    //TODO caluculate angle
    timer.start(16); //update position every 10ms

    QObject::connect(&timer, SIGNAL(timeout()), this, SLOT(update()));
}


void basicBullet::advance(int phase){

}

void basicBullet::update(){
    setPos(x()+m_Speed*m_DirX, y()+m_Speed*m_DirY);
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
