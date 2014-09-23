#ifndef BASICBULLET_H
#define BASICBULLET_H

#include "basebullet.h"

class basicBullet : public baseBullet
{
public:
    basicBullet(baseEnemy* target, baseTower* tower);
    virtual void advance(int phase);

public slots:
    virtual void update();

private:
    qreal angle;
    qreal m_DirX;
    qreal m_DirY;
};

#endif // BASICBULLET_H
