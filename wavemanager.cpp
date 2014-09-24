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
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;

    case 2:
        timer.setInterval(300);
        if(enemyNr < 30){
            setUpEnemy(new baseEnemy(1, helper));
            enemyNr++;
        } else {
            setUpEnemy(new baseEnemy(2, helper));
            enemyNr++;
        }
        if(enemyNr > 45){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;

    case 3:
        timer.setInterval(300);
        if(enemyNr < 15){
            setUpEnemy(new baseEnemy(1, helper));
            enemyNr++;
        } else if(enemyNr < 30  ){
            setUpEnemy(new baseEnemy(2, helper));
            enemyNr++;
        }
        else if(enemyNr < 34  )
            setUpEnemy(new baseEnemy(3, helper));
        enemyNr++;
        if(enemyNr > 34){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 4:
        timer.setInterval(300);
        if(enemyNr < 10){
            setUpEnemy(new baseEnemy(1, helper));
            enemyNr++;
        } else if(enemyNr < 30) {
            setUpEnemy(new baseEnemy(2, helper));
            enemyNr++;
        }
        else if(enemyNr < 44  )
            setUpEnemy(new baseEnemy(3, helper));
        enemyNr++;
        if(enemyNr > 44){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 5:
        timer.setInterval(300);
        if(enemyNr < 102){
            setUpEnemy(new baseEnemy(2, helper));
            enemyNr++;
        }
        if(enemyNr > 102){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 6:
        timer.setInterval(300);
        if(enemyNr < 15){
            setUpEnemy(new baseEnemy(2, helper));
            enemyNr++;
        } else if(enemyNr < 25) {
            setUpEnemy(new baseEnemy(3, helper));
            enemyNr++;
        }
        else if(enemyNr < 30) {
        setUpEnemy(new baseEnemy(4, helper));
        enemyNr++;
        }
        if(enemyNr > 30){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
    break;
    case 7:
        timer.setInterval(300);
        if(enemyNr < 49){
            setUpEnemy(new baseEnemy(2, helper));
            enemyNr++;
        } else if(enemyNr < 64) {
            setUpEnemy(new baseEnemy(3, helper));
            enemyNr++;
        }
         else if(enemyNr < 74) {
        setUpEnemy(new baseEnemy(4, helper));
        enemyNr++;
        }
         else if(enemyNr < 83) {
        setUpEnemy(new baseEnemy(5, helper));
        enemyNr++;
        }
        if(enemyNr > 83){
            enemyNr = 0;
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
