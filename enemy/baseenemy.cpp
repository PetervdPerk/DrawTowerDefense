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
    animation->setKeyValues(*helper->getPositions());
    animation->setEndValue(helper->getEndPosition());
    animation->start();

    QObject::connect(animation, SIGNAL(stateChanged(QAbstractAnimation::State,QAbstractAnimation::State)),
                     this, SLOT(animationStateChanged(QAbstractAnimation::State,QAbstractAnimation::State)));
}

void baseEnemy::animationStateChanged(QAbstractAnimation::State newState,QAbstractAnimation::State oldstate){
    if(newState == QAbstractAnimation::Stopped){
        this->setEnabled(false);
        this->setVisible(false);
        emit removed(this);
    }
}

bool baseEnemy::getKilled(){
    return killed;
}

void baseEnemy::setCenterRect(QPointF position){
    //Set device to center of location
    QRectF rect = this->rect();
    rect.moveCenter(position);
    setRect(rect);
}

int baseEnemy::getAnimationTime(){
    return animation->currentTime();
}

void baseEnemy::hit(qreal damage){
    health -= damage;
    if(health <= 0){
        this->setEnabled(false);
        this->setVisible(false);
        animation->pause();
        killed = true;
        emit removed(this);
    }
}
