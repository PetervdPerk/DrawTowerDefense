#ifndef GAME_H
#define GAME_H

#include <QObject>
#include "enemy/baseenemy.h"
#include "enemy/enemyhelper.h"
#include "tower/basetower.h"
#include "tower/lasertower.h"
#include "wavemanager.h"
#include "view.h"
#include "gameUIenum.h"
#include "gameline.h"
#include <QPropertyAnimation>
#include <QPainterPath>
#include <QGraphicsPathItem>
#include <QDebug>
#include <QList>
#include <QHash>
#include <QTimer>


class game : public QObject
{
    Q_OBJECT
public:
    game(View* gameView, QObject *parent = 0);
    qreal getLife();
    qreal getMoney();
    qreal getWave();
  //  qreal getWaveTimer();

signals:
    void updateUI(UIupdate upd);

public slots:
    void enemyDown(baseEnemy *enemy);
    void wave();
    void UpdateMoney();
    void glyphLoc(QPointF loc, int id);
    void drawLine(QPolygonF line);
    void startDraw();
//    void WaveTimer();

private:
    void addTower(int id);
    void addLaserTower(int id);

    View* gameView;
    QHash<int, baseTower*> towers;
    QList<baseEnemy*> enemies;
    enemyHelper* enemyHelp;
    waveManager* wavesMgr;
    QPainterPath path;
    bool paintOnce = true;
    QTimer moneyTimer;

    qreal life = 500;
    qreal money = 2000;
    qreal wave_stat = 1;

    bool gameStarted = false;

  //  qreal wave_timer = 0;

};

#endif // GAME_H
