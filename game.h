#ifndef GAME_H
#define GAME_H

#include <QObject>
#include "enemy/baseenemy.h"
#include "enemy/enemyhelper.h"
#include "tower/basetower.h"
#include "wavemanager.h"
#include "view.h"
#include <QPropertyAnimation>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QDebug>
#include <QList>


class game : public QObject
{
    Q_OBJECT
public:
    game(View* gameView, QObject *parent = 0);

signals:

public slots:
    void enemyDown(baseEnemy *enemy);

private:
    void addTower(qreal x = 0, qreal y = 0);

    View* gameView;
    QList<baseEnemy*> enemies;
    QList<baseTower*> towers;
    enemyHelper* enemyHelp;
    waveManager* wavesMgr;
    QPainterPath path;

};

#endif // GAME_H
