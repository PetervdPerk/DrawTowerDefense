#ifndef ENEMYHELPER_H
#define ENEMYHELPER_H

#include <QObject>
#include <QPropertyAnimation>
#include <QPainterPath>
#include <QVector>

class enemyHelper : public QObject
{
    Q_OBJECT
public:
    enemyHelper(QPainterPath *gamePath, QObject *parent = 0);
    QVariantAnimation::KeyValues* getPositions();
    qreal getDurationOfPath(int health);
    QPointF getEndPosition();

signals:

public slots:

private:
    QPainterPath *gamePath;
    QVariantAnimation::KeyValues positions;
    QPointF endPosition;
    QVector<qreal> durations;
};

#endif // ENEMYHELPER_H
