#ifndef BASEENEMY_H
#define BASEENEMY_H

#include <QGraphicsEllipseItem>
#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QPropertyAnimation>
#include <QPainter>
#include <QBrush>
#include <QPen>

#include "enemyhelper.h"

class baseEnemy : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setCenterRect)
public:
    baseEnemy(qreal health, enemyHelper* helper, QGraphicsItem * parent = 0);
    void setCenterRect(QPointF position);
    void hit(qreal damage);
    int getAnimationTime();
    bool getKilled();

protected:

public slots:
    void animationStateChanged(QAbstractAnimation::State newState,QAbstractAnimation::State oldstate);

signals:
    void removed(baseEnemy* enemy);

private:
    qreal angle, speed;
    enemyHelper* helper;
    QPropertyAnimation* animation;
    qreal health;
    bool killed = false;
};

#endif // BASEENEMY_H
