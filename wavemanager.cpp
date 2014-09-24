#include "wavemanager.h"

#define SEC 1000
#define TIME_BETWEEN_WAVES 10*SEC

waveManager::waveManager(QList<baseEnemy*> *enemies, View *gameView, enemyHelper *helper, QObject *parent) :
    QObject(parent)
{
    this->enemies = enemies;
    this->gameView = gameView;
    this->helper = helper;

    QObject::connect(&timer, SIGNAL(timeout()),this, SLOT(wave()));
    timer.start(SEC * 1);

    emit newWave();
}

void waveManager::wave(){
    if(processWave()){ //Wave done
        emit newWave();
        wave_nr++;
    }
}

void waveManager::setUpEnemy(baseEnemy *en){

    QObject::connect(en, SIGNAL(removed(baseEnemy*)), this, SLOT(enemyRemoved(baseEnemy*)));

    enemies->append(en);
    gameView->addItem(en);
}

bool waveManager::processWave(){
    bool new_wave = false;

    switch(wave_nr){

    case 1: //Wave
        timer.setInterval(300);
        setUpEnemy(new baseEnemy(1, helper));
        enemyNr++;
        if(enemyNr > 30){
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;

    case 2:
        timer.setInterval(300);
        setUpEnemy(new baseEnemy(1, helper));
        enemyNr++;
        if(enemyNr > 40){
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;

    default:
        break;

    }

    return new_wave;

}


void waveManager::enemyRemoved(baseEnemy *enemy){
    emit enemyDown(enemy);
}
