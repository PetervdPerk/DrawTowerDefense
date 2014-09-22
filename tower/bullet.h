#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QPropertyAnimation>
#include <QTimer>
#include <QBrush>
#include <QPen>

#include <QDebug>
#include <math.h>
#include "enemy/baseenemy.h"
#include "basetower.h"

class Bullet : public QObject, public QGraphicsEllipseItem
// class for the fire bullets
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setCenterRect)
public:
    Bullet(baseEnemy* target, baseTower* tower);
    virtual ~Bullet() {}
    void setCenterRect(QPointF position);
    void advance(int phase);

private:
    QBrush m_Brush;
    qreal m_Speed;
    baseEnemy* target;
    QPropertyAnimation* animation;
};
