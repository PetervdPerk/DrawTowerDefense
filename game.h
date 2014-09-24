#ifndef GAME_H
#define GAME_H

#include <QObject>
#include "enemy/baseenemy.h"
#include "enemy/enemyhelper.h"
#include "tower/basetower.h"
#include "wavemanager.h"
#include "view.h"
#include "gameUIenum.h"
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
    qreal getLife();
    qreal getMoney();
    qreal getWave();

signals:
    void updateUI(UIupdate upd);

public slots:
    void enemyDown(baseEnemy *enemy);
    void wave();

private:
    void addTower(qreal x = 0, qreal y = 0);

    View* gameView;
    QList<baseEnemy*> enemies;
    QList<baseTower*> towers;
    enemyHelper* enemyHelp;
    waveManager* wavesMgr;
    QPainterPath path;

    qreal life = 100;
    qreal money = 2000;
    qreal wave_stat = 1;

};

#endif // GAME_H
