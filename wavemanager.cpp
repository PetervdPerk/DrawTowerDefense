#include "wavemanager.h"

#define SEC 1000

waveManager::waveManager(QList<baseEnemy*> *enemies, View *gameView, enemyHelper *helper, QObject *parent) :
    QObject(parent)
{
    this->enemies = enemies;
    this->gameView = gameView;
    this->helper = helper;

    QObject::connect(&timer, SIGNAL(timeout()),this, SLOT(wave()));
    timer.start(SEC * 1);
}

void waveManager::wave(){

    baseEnemy *en = new baseEnemy(1, helper);
    QObject::connect(en, SIGNAL(removed(baseEnemy*)), this, SLOT(enemyRemoved(baseEnemy*)));

    enemies->append(en);
    gameView->addItem(en);

    emit newWave();
}


void waveManager::enemyRemoved(baseEnemy *enemy){
    emit enemyDown(enemy);
}
