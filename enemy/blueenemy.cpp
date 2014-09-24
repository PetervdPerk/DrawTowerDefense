#include "blueenemy.h"

blueenemy::blueenemy(qreal health, enemyHelper* helper, QGraphicsItem * parent)
    : baseEnemy(health, helper, parent)
{
        this->setBrush(QBrush(QColor("Blue")));
}

void blueenemy::hit(qreal damage) {
    health -= damage;
    if(health <= 0){
        this->setEnabled(false);
        this->setVisible(false);
        animation->pause();
        killed = true;
        emit removed(this);
    }
}
