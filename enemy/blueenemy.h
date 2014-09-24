#ifndef BLUEENEMY_H
#define BLUEENEMY_H
#include "baseenemy.h"

class blueenemy : public baseEnemy
{
public:
    blueenemy(qreal health, enemyHelper* helper, QGraphicsItem * parent = 0);
    void hit(qreal damage);


};

#endif // BLUEENEMY_H
