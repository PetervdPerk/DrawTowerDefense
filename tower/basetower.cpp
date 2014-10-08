#include "tower/basetower.h"
#include <QDebug>
#include <QtCore/qmath.h>

#include "bullet/basicbullet.h"
#include "bullet/guidancebullet.h"
#include "bullet/laserbullet.h"



baseTower::baseTower(qreal x, qreal y, QList<baseEnemy*> *enemies, QGraphicsItem * parent) :QGraphicsRectItem(x,y,68,68, parent)
{
    this->enemies = enemies;
    setPos(x,y);
    this->setBrush(QBrush(QColor(255,255,255)));
    this->setVisible(true);

    rangeEllipse = new QGraphicsEllipseItem(this);

    setRange(128);

    QObject::connect(&shootTimer, SIGNAL(timeout()), this, SLOT(shoot()));
    shootTimer.start(500);
}

void baseTower::setCenterRect(QPointF position){
    //Set device to location
    QRectF rect = this->rect();
    rect.moveCenter(position);
    setRect(rect);
}

void baseTower::setCenterPos(QPointF position){
    //Set device to location
    position.setX(position.x() - (size / 2));
    position.setY(position.y() - (size / 2));

    setPos(position);
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
        baseBullet *bull = new basicBullet(targetEnemy,this);
    }
}
