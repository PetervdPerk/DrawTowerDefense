#include "tower/lasertower.h"
#include <QDebug>
#include <QtCore/qmath.h>

#include "bullet/basicbullet.h"
#include "bullet/guidancebullet.h"
#include "bullet/laserbullet.h"


laserTower::laserTower(qreal x, qreal y, QList<baseEnemy*> *enemies, QGraphicsItem * parent) :baseTower(x,y,enemies)
//laserTower::laserTower(qreal x, qreal y, QList<baseEnemy*> *enemies, QGraphicsItem * parent) :baseTower(x,y,enemies), QGraphicsEllipseItem(x,y,32,32, parent)
//laserTower::laserTower(qreal x, qreal y) : baseTower(x,y)
{

    setPos(x,y);
    this->setBrush(QBrush(QColor(0,0,255)));
    this->setVisible(true);

   setRange(192);

    QObject::connect(&shootTimer, SIGNAL(timeout()), this, SLOT(shoot()));
    shootTimer.setInterval(1000);
}

void laserTower::setCenterRect(QPointF position){
    //Set device to location
    QRectF rect = this->rect();
    rect.moveCenter(position);
    setRect(rect);
}

void laserTower::setRange(qreal range){
    this->range = range;
    QRectF rangeRect;
    rangeRect.setWidth(range*2);
    rangeRect.setHeight(range*2);
    rangeRect.moveCenter(this->rect().center());
    rangeEllipse->setRect(rangeRect);
}

void laserTower::advance(int phase){
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
void laserTower::mousePressEvent(QGraphicsSceneMouseEvent *event){
    qDebug() << "Mouse click";
    advance(0);
}

void laserTower::shoot(){
    //Collision method 3
    qreal x;
    qreal y;
    qreal distance;
    int animationTime = 0;
    int biggestAnimationTime = 0;
    //qreal shortestDistance = range;
    baseEnemy* enemy;
    baseEnemy* targetEnemy = NULL;

    //Range based
    /*
    for(int i = 0; i < enemies->size(); i++) {
        enemy = enemies->at(i);
        x = enemy->rect().center().x() - pos().x();
        y = enemy->rect().center().y() - pos().y();

        distance = qSqrt(x * x + y * y);

        if(distance < shortestDistance){
            targetEnemy = enemy;
            shortestDistance = distance;
        }
    }*/

    //Loc based
    for(int i = 0; i < enemies->size(); i++) {
        enemy = enemies->at(i);

        x = enemy->rect().center().x() - (pos().x() + rect().width()/2);
        y = enemy->rect().center().y() - (pos().y() + rect().height()/2);

        distance = qSqrt(x * x + y * y);

        animationTime = enemy->getAnimationTime();

        if(distance < range && animationTime > biggestAnimationTime ){
            targetEnemy = enemy;
            biggestAnimationTime = animationTime;
        }
    }

    if(targetEnemy != NULL){
        baseBullet *bull = new laserBullet(targetEnemy,this);
    }
}

