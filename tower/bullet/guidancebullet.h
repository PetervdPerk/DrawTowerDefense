#ifndef GUIDANCEBULLET_H
#define GUIDANCEBULLET_H

#include <QPropertyAnimation>
#include "basebullet.h"

class guidanceBullet : public baseBullet
{
public:
    guidanceBullet(baseEnemy* target, baseTower* tower = 0);
    virtual void advance(int phase);

private:
    QPropertyAnimation* animation;
};

#endif // GUIDANCEBULLET_H
