#ifndef WAVEMANAGER_H
#define WAVEMANAGER_H

#include <QObject>
#include <QTimer>
#include <QList>
#include "view.h"
#include "enemy/baseenemy.h"
#include "enemy/blueenemy.h"
#include "enemy/enemyhelper.h"

class waveManager : public QObject
{
    Q_OBJECT
public:
    waveManager(QList<baseEnemy*> *enemies, View *gameView, enemyHelper *helper, QObject *parent = 0);

signals:
    void newWave();
    void enemyDown(baseEnemy *enemy);

public slots:
    void wave();
    void enemyRemoved(baseEnemy *enemy);
    bool processWave();

private:
    void setUpEnemy(baseEnemy *en);

    int wave_nr = 1;
    qreal enemyNr = 0;
    QTimer timer;
    QList<baseEnemy*> *enemies;
    View *gameView;
    enemyHelper *helper;

};

#endif // WAVEMANAGER_H
