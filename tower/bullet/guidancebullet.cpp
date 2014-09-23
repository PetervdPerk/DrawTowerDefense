#include "guidancebullet.h"

guidanceBullet::guidanceBullet(baseEnemy* target, baseTower* tower)
    : baseBullet(target,tower)
{

    animation = new QPropertyAnimation(this,"pos");

    animation->setDuration(1000);
    QRectF towerPos = tower->rect();
    towerPos.setTopLeft(tower->pos());
    animation->setStartValue(tower->pos());
    animation->setEndValue(target->rect().center());
    animation->start();

    QObject::connect(animation, SIGNAL(finished()), this, SLOT(deleteLater()));
}

void guidanceBullet::advance(int phase){
    if(target != NULL){
        //TODO this is very inefficient
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
