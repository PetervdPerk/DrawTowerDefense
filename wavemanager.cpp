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
        } else {
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 6:
        timer.setInterval(300);
        if(enemyNr < 15){
            setUpEnemy(new baseEnemy(2, helper));
        } else if(enemyNr < 25) {
            setUpEnemy(new baseEnemy(3, helper));
        }
        else if(enemyNr < 30) {
            setUpEnemy(new baseEnemy(4, helper));
        }
        enemyNr++;
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
        } else if(enemyNr < 64) {
            setUpEnemy(new baseEnemy(3, helper));
        }
        else if(enemyNr < 74) {
            setUpEnemy(new baseEnemy(4, helper));
        }
        else if(enemyNr < 83) {
            setUpEnemy(new baseEnemy(5, helper));
        }
        enemyNr++;
        if(enemyNr > 83){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 8:
        timer.setInterval(300);
        if(enemyNr < 20){
            setUpEnemy(new baseEnemy(3, helper));
        } else if(enemyNr < 28) {
            setUpEnemy(new baseEnemy(4, helper));
        }
        else if(enemyNr < 32) {
            setUpEnemy(new baseEnemy(5, helper));
        }
        enemyNr++;
        if(enemyNr > 32){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 9:
        timer.setInterval(300);
        if(enemyNr < 80){
            setUpEnemy(new baseEnemy(3, helper));
        }
        enemyNr++;
        if(enemyNr > 80){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 10:
        timer.setInterval(300);
        if(enemyNr < 6){
            setUpEnemy(new baseEnemy(6, helper));
        }
        enemyNr++;
        if(enemyNr > 6){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 11:
        timer.setInterval(300);
        if(enemyNr < 8){
            setUpEnemy(new baseEnemy(7, helper));
        }
        enemyNr++;
        if(enemyNr > 8){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 12:
        timer.setInterval(300);
        if(enemyNr < 31){
            setUpEnemy(new baseEnemy(4, helper));
        }
        enemyNr++;
        if(enemyNr > 31){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 13:
        timer.setInterval(300);
        if(enemyNr < 23){
            setUpEnemy(new baseEnemy(5, helper));
        }
        else if(enemyNr < 27) {
            setUpEnemy(new baseEnemy(1, helper));//should be zebra
        }
        enemyNr++;
        if(enemyNr > 27){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 14:
        timer.setInterval(300);
        if(enemyNr < 4){
            setUpEnemy(new baseEnemy(1, helper));//should be leads
        }
        enemyNr++;
        if(enemyNr > 4){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 15:
        timer.setInterval(300);
        if(enemyNr < 9){
            setUpEnemy(new baseEnemy(1, helper)); //should be leads
        }
        enemyNr++;
        if(enemyNr > 9){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 16:
        timer.setInterval(300);
        if(enemyNr < 25){
            setUpEnemy(new baseEnemy(6, helper));
        }
        else if(enemyNr < 53) {
            setUpEnemy(new baseEnemy(7, helper));
        }
        else if(enemyNr < 61) {
            setUpEnemy(new baseEnemy(1, helper));//should be leads
        }
        enemyNr++;
        if(enemyNr > 61){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 17:
        timer.setInterval(300);
        if(enemyNr < 140){
            setUpEnemy(new baseEnemy(4, helper));
        }
        else if(enemyNr < 145) {
            setUpEnemy(new baseEnemy(1, helper)); //should be zebras
        }
        enemyNr++;
        if(enemyNr > 145){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 18:
        timer.setInterval(300);
        if(enemyNr < 81){
            setUpEnemy(new baseEnemy(5, helper));
        }
        enemyNr++;
        if(enemyNr > 81){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 19:
        timer.setInterval(300);
        if(enemyNr < 42){
            setUpEnemy(new baseEnemy(5, helper));
        }
        else if(enemyNr < 59) {
            setUpEnemy(new baseEnemy(7, helper));
        }
        else if(enemyNr < 73) {
            setUpEnemy(new baseEnemy(1, helper)); //should be leads
        }
        else if(enemyNr < 83) {
            setUpEnemy(new baseEnemy(1, helper)); //should be zebras
        }
        else if(enemyNr < 87) {
            setUpEnemy(new baseEnemy(1, helper)); //should be rainbows
        }
        enemyNr++;
        if(enemyNr > 87){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 20:
        timer.setInterval(300);
        if(enemyNr < 10){
            setUpEnemy(new baseEnemy(1, helper));//should be rainbows
        }
        else if(enemyNr < 14) {
            setUpEnemy(new baseEnemy(1, helper)); //should be ceramics
        }
        enemyNr++;
        if(enemyNr > 14){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 21:
        timer.setInterval(300);
        if(enemyNr < 6){
            setUpEnemy(new baseEnemy(1, helper));//should be rainbows
        }
        else if(enemyNr < 12) {
            setUpEnemy(new baseEnemy(1, helper)); //should be ceramics
        }
        enemyNr++;
        if(enemyNr > 12){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 22:
        timer.setInterval(300);
        if(enemyNr < 50){
            setUpEnemy(new baseEnemy(1, helper));//should be zebras
        }
        enemyNr++;
        if(enemyNr > 50){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 23:
        timer.setInterval(300);
        if(enemyNr < 1){
            setUpEnemy(new baseEnemy(1, helper));//should be M.O.A.B.
        }
        enemyNr++;
        if(enemyNr > 1){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 24:
        timer.setInterval(300);
        if(enemyNr < 120){
            setUpEnemy(new baseEnemy(5, helper));
        }
        else if(enemyNr < 170) {
            setUpEnemy(new baseEnemy(1, helper)); //should be rainbows
        }
        enemyNr++;
        if(enemyNr > 170){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 25:
        timer.setInterval(300);
        if(enemyNr < 1){
            setUpEnemy(new baseEnemy(1, helper));//should be Moab
        }
        else if(enemyNr < 9) {
            setUpEnemy(new baseEnemy(1, helper)); //should be leads
        }
        else if(enemyNr < 29) {
            setUpEnemy(new baseEnemy(1, helper));
        }
        else if(enemyNr < 49) {
            setUpEnemy(new baseEnemy(1, helper)); //should be ceramics
        }
        else if(enemyNr < 50) {
            setUpEnemy(new baseEnemy(1, helper)); //should be moab
        }
        enemyNr++;
        if(enemyNr >50){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 26:
        timer.setInterval(300);
        if(enemyNr < 25){
            setUpEnemy(new baseEnemy(1, helper)); //should be rainbows
        }
        else if(enemyNr < 35) {
            setUpEnemy(new baseEnemy(1, helper)); //should be ceramics
        }
        else if(enemyNr < 37) {
            setUpEnemy(new baseEnemy(1, helper)); //should be moabs
        }
        enemyNr++;
        if(enemyNr > 37){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 27:
        timer.setInterval(300);
        if(enemyNr < 35){
            setUpEnemy(new baseEnemy(1, helper)); //should be ceramics
        }
        else if(enemyNr < 37) {
            setUpEnemy(new baseEnemy(1, helper)); //should be moabs
        }
        enemyNr++;
        if(enemyNr > 37){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 28:
        timer.setInterval(300);
        if(enemyNr < 40){
            setUpEnemy(new baseEnemy(1, helper)); //should be raibows
        }
        else if(enemyNr < 41) {
            setUpEnemy(new baseEnemy(1, helper)); //should be moabs
        }
        enemyNr++;
        if(enemyNr > 41){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 29:
        timer.setInterval(300);
        if(enemyNr < 29){
            setUpEnemy(new baseEnemy(1, helper)); //should be ceramics
        }
        else if(enemyNr < 34) {
            setUpEnemy(new baseEnemy(1, helper)); //should be moabs
        }
        enemyNr++;
        if(enemyNr > 34){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 30:
        timer.setInterval(300);
        if(enemyNr < 1){
            setUpEnemy(new baseEnemy(1, helper)); //should be B.F.B
        }
        enemyNr++;
        if(enemyNr > 1){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 31:
        timer.setInterval(300);
        if(enemyNr < 300){
            setUpEnemy(new baseEnemy(5, helper));
        }
        else if(enemyNr < 315) {
            setUpEnemy(new baseEnemy(1, helper)); //should be rainbows
        }
        else if(enemyNr < 321) {
            setUpEnemy(new baseEnemy(1, helper)); //should be moabs
        }
        enemyNr++;
        if(enemyNr > 321){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 32:
        timer.setInterval(300);
        if(enemyNr < 9){
            setUpEnemy(new baseEnemy(1, helper));//should be moabs
        }
        enemyNr++;
        if(enemyNr > 9){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 33:
        timer.setInterval(300);
        if(enemyNr < 12){
            setUpEnemy(new baseEnemy(1, helper));//should be moabs
        }
        enemyNr++;
        if(enemyNr > 12){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 34:
        timer.setInterval(300);
        if(enemyNr < 4){
            setUpEnemy(new baseEnemy(1, helper));//should be moabs
        }
        else if(enemyNr < 5){
            setUpEnemy(new baseEnemy(1, helper));//should be bfb
        }
        enemyNr++;
        if(enemyNr > 5){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 35:
        timer.setInterval(300);
        if(enemyNr < 200){
            setUpEnemy(new baseEnemy(1, helper));//should be rainbows
        }
        else if(enemyNr < 204){
            setUpEnemy(new baseEnemy(1, helper));//should be moabs
        }
        enemyNr++;
        if(enemyNr > 204){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 36:
        timer.setInterval(300);
        if(enemyNr < 38){
            setUpEnemy(new baseEnemy(1, helper));//should be ceramics
        }
        else if(enemyNr < 40){
            setUpEnemy(new baseEnemy(1, helper));//should be bfb
        }
        enemyNr++;
        if(enemyNr > 40){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 37:
        timer.setInterval(300);
        if(enemyNr < 200){
            setUpEnemy(new baseEnemy(1, helper));//should be ceramics
        }
        else if(enemyNr < 201){
            setUpEnemy(new baseEnemy(1, helper));//should be bfb
        }
        enemyNr++;
        if(enemyNr > 201){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 38:
        timer.setInterval(300);
        if(enemyNr < 60){
            setUpEnemy(new baseEnemy(1, helper));//should be ceramics
        }
        enemyNr++;
        if(enemyNr > 60){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 39:
        timer.setInterval(300);
        if(enemyNr < 150){
            setUpEnemy(new baseEnemy(1, helper));//should be rainbows
        }
        else if(enemyNr < 225){
            setUpEnemy(new baseEnemy(1, helper));//should be ceramics
        }
        else if(enemyNr < 297){
            setUpEnemy(new baseEnemy(1, helper));//should be ceramics
        }
        else if(enemyNr < 298){
            setUpEnemy(new baseEnemy(1, helper));//should be bfbs
        }
        enemyNr++;
        if(enemyNr > 298){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 40:
        timer.setInterval(300);
        if(enemyNr < 31){
            setUpEnemy(new baseEnemy(1, helper));//should be moabs
        }
        enemyNr++;
        if(enemyNr > 31){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 41:
        timer.setInterval(300);
        if(enemyNr < 400){
            setUpEnemy(new baseEnemy(1, helper));//should be rainbows
        }
        else if(enemyNr < 410){
            setUpEnemy(new baseEnemy(1, helper));//should be bfbs
        }
        enemyNr++;
        if(enemyNr > 410){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 42:
        timer.setInterval(300);
        if(enemyNr < 50){
            setUpEnemy(new baseEnemy(1, helper));//should be moabs
        }
        else if(enemyNr < 60){
            setUpEnemy(new baseEnemy(1, helper));//should be bfbs
        }
        enemyNr++;
        if(enemyNr > 60){
            enemyNr = 0;
            new_wave = true;
            timer.setInterval(TIME_BETWEEN_WAVES);
        }
        break;
    case 43:
        timer.setInterval(300);
        if(enemyNr < 1){
            setUpEnemy(new baseEnemy(1, helper));//should be ZOMG
        }
        enemyNr++;
        if(enemyNr > 1){
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
