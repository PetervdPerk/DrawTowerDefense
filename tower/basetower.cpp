#include "tower/basetower.h"
#include <QDebug>
#include <QtCore/qmath.h>

#include "bullet/basicbullet.h"
#include "bullet/guidancebullet.h"

baseTower::baseTower(qreal x, qreal y, QList<baseEnemy*> *enemies, QGraphicsItem * parent) :QGraphicsEllipseItem(x,y,32,32, parent)
{
    this->enemies = enemies;
    setPos(x,y);
    this->setBrush(QBrush(QColor(255,0,0)));
    this->setVisible(true);

    rangeEllipse = new QGraphicsEllipseItem(this);

    setRange(128);

    QObject::connect(&shootTimer, SIGNAL(timeout()), this, SLOT(shoot()));
    shootTimer.start(1000);
}

void baseTower::setCenterRect(QPointF position){
    //Set device to location
    QRectF rect = this->rect();
    rect.moveCenter(position);
    setRect(rect);
}

void baseTower::setRange(qreal range){
    this->range = range;
    QRectF rangeRect;
    rangeRect.setWidth(range*2);
    rangeRect.setHeight(range*2);
    rangeRect.moveCenter(this->rect().center());
    rangeEllipse->setRect(rangeRect);
}

void baseTower::advance(int phase){
    //Collision method 1
    /*QList<QGraphicsItem *> list = rangeEllipse->collidingItems();

    if(list.size() > 1){
        foreach(QGraphicsItem * i , list)
        {
            baseEnemy* item = dynamic_cast<baseEnemy*>(i);
            if (item)
            {
                Bullet *bull = new Bullet(item,this);
            }
        }
    }*/

    //Collision method 2
    /*
    for(int i = 0; i < enemies->size(); i++) {
        baseEnemy* en = enemies->at(i);
        if(rangeEllipse->collidesWithItem(en)) {
            Bullet *bull = new Bullet(en,this);
        }
    }*/
    /*
    //Collision method 3
    for(int i = 0; i < enemies->size(); i++) {
        baseEnemy* en = enemies->at(i);
        qreal x = en->rect().x() - pos().x();
        qreal y = en->rect().y() - pos().y();

        qreal distance = qSqrt(x * x + y * y);

        //qDebug() << "Enemy: " << x << " " << y << " " << distance << range;

        if(distance < range){
            baseBullet *bull = new basicBullet(en,this);
        }
    }*/
}
void baseTower::mousePressEvent(QGraphicsSceneMouseEvent *event){
    qDebug() << "Mouse click";
    advance(0);
}

void baseTower::shoot(){
    //Collision method 3
    qreal x;
    qreal y;
    qreal distance;
    qreal shortestDistance = range;
    baseEnemy* enemy;
    baseEnemy* closestEnemy = NULL;

    for(int i = 0; i < enemies->size(); i++) {
        enemy = enemies->at(i);
        x = enemy->rect().center().x() - pos().x();
        y = enemy->rect().center().y() - pos().y();

        distance = qSqrt(x * x + y * y);

        if(distance < shortestDistance){
            closestEnemy = enemy;
            shortestDistance = distance;
        }
    }

    if(closestEnemy != NULL){
        baseBullet *bull = new basicBullet(closestEnemy,this);
    }
}
