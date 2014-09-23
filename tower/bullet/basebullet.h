#ifndef BASEBULLET_H
#define BASEBULLET_H


#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>
#include <QBrush>
#include <QPen>

#include <QDebug>
#include <math.h>
#include <QTimer>
#include "enemy/baseenemy.h"
#include "tower/basetower.h"

class baseBullet : public QObject, public QGraphicsEllipseItem
// class for the fire baseBullets
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setCenterRect)
public:
    baseBullet(baseEnemy* target, baseTower* tower);
    virtual ~baseBullet() {}
    void setCenterRect(QPointF position);
    virtual void advance(int phase) = 0;

public slots:
    virtual void update(){};

protected:
    baseEnemy* target;
    QBrush m_Brush;
    qreal m_Speed;
    QTimer timer;
};

#endif // BASEBULLET_H
