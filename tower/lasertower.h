#ifndef LASERTOWER_H
#define LASERTOWER_H

#include "basetower.h"
class laserTower : public baseTower
{
public:
    laserTower(qreal x, qreal y, QList<baseEnemy*> *enemies, QGraphicsItem * parent = 0);
    //laserTower(qreal x, qreal y, QList<baseEnemy*> *enemies);
    //laserTower(qreal x, qreal y);
    void setCenterRect(QPointF position);
    void setRange(qreal range);
    void advance(int phase);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    QTimer shootTimer;

public slots:
    virtual void shoot();

};

#endif // LASERTOWER_H
