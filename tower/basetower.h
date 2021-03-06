#ifndef BASETOWER_H
#define BASETOWER_H


#include "enemy/baseenemy.h"
#include <QGraphicsEllipseItem>
#include <QGraphicsObject>
#include <QGraphicsScene>
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <QTimer>

class baseTower : public QObject, public QGraphicsRectItem
{
    Q_OBJECT
    Q_PROPERTY(QPointF pos READ pos WRITE setCenterRect)
public:
    baseTower(qreal x, qreal y, QList<baseEnemy*> *enemies, QGraphicsItem * parent = 0);
    void setCenterRect(QPointF position);
    void setCenterPos(QPointF position);
    void setRange(qreal range);
    void advance(int phase);


protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    QTimer shootTimer;
    QGraphicsEllipseItem *rangeEllipse;
    QList<baseEnemy*> *enemies;
    qreal angle, speed;
    qreal range;

public slots:
    virtual void shoot();

private:
    const int size = 68;
   // QGraphicsEllipseItem *rangeEllipse;
    //QList<baseEnemy*> *enemies;
   // qreal angle, speed;
   // qreal range;
};

#endif // BASETOWER_H
