#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QTimer>
#include <QBrush>
#include <QPen>

#include <QDebug>
#include <math.h>
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
    void advance(int phase);

protected:
    baseEnemy* target;
    QBrush m_Brush;
    qreal m_Speed;
};
