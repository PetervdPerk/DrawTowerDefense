#ifndef LASERBULLET_H
#define LASERBULLET_H

#include "basebullet.h"

class laserBullet : public baseBullet
{
public:
    laserBullet(baseEnemy* target, baseTower* tower);
    virtual void advance(int phase);

public slots:
    virtual void update();

private:
    qreal angle;
    qreal m_DirX;
    qreal m_DirY;
};

#endif // LASERBULLET_H

