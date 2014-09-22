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

    enemies->append(en);
    gameView->addItem(en);

    emit newWave();
}


void waveManager::enemyRemoved(bool destroyed){
    if(destroyed){
        emit enemyDown();
    } else {
        emit lifeDown();
    }
}
