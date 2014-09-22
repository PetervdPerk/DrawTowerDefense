#ifndef ENEMYHELPER_H
#define ENEMYHELPER_H

#include <QObject>
#include <QPropertyAnimation>
#include <QPainterPath>

class enemyHelper : public QObject
{
    Q_OBJECT
public:
    enemyHelper(QPainterPath *gamePath, QObject *parent = 0);
    QVariantAnimation::KeyValues* getPositions();
    qreal getDurationOfPath();

signals:

public slots:

private:
    QPainterPath *gamePath;
    QVariantAnimation::KeyValues positions;
};

#endif // ENEMYHELPER_H
