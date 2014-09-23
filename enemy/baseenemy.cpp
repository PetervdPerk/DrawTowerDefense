#include "enemy/baseenemy.h"
#include <QDebug>

baseEnemy::baseEnemy(qreal health, enemyHelper* helper, QGraphicsItem * parent) :QGraphicsEllipseItem(parent)
{
    this->helper = helper;
    this->health = health;

    setRect(-20.0,-20.0,30.0,30.0);
    this->setBrush(QBrush(QColor(255,0,0)));
    this->setVisible(true);

    //Animation to follow line
    animation = new QPropertyAnimation(this,"pos",this);
    animation->setDuration(helper->getDurationOfPath());
    animation->setEasingCurve(QEasingCurve::Linear);
    animation->setLoopCount(-1); //loop forever
    animation->setKeyValues(*helper->getPositions());
    animation->start();
}

void baseEnemy::setCenterRect(QPointF position){
    //Set device to center of location
    QRectF rect = this->rect();
    rect.moveCenter(position);
    setRect(rect);
}

void baseEnemy::hit(qreal damage){
    health -= damage;
    if(health <= 0){
        this->setEnabled(false);
        this->setVisible(false);

        emit removed(this);
    }
}
