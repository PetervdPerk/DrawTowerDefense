#include "game.h"

game::game(View* gameView, QObject *parent) :
    QObject(parent)
{
    this->gameView = gameView;

    path.moveTo(000,100);
    path.quadTo(100,600,400,400);
    path.quadTo(100,750,750,480);
    path.quadTo(200,50,300,0);

    QGraphicsPathItem* path2 = new QGraphicsPathItem(path);

    gameView->addItem(path2);

    enemyHelp = new enemyHelper(&path);
    wavesMgr = new waveManager(&enemies,gameView,enemyHelp,this);

    QObject::connect(wavesMgr, SIGNAL(newWave()), this, SLOT(wave()));
    QObject::connect(wavesMgr, SIGNAL(enemyDown(baseEnemy*)), this, SLOT(enemyDown(baseEnemy*)));

    addTower();

    addTower();
}


void game::addTower(qreal x, qreal y) {
    baseTower* tower = new baseTower(x,y,&enemies);
    tower->setFlag(QGraphicsItem::ItemIsMovable, true);
    towers.append(tower);
    gameView->addItem(tower);
}

void game::enemyDown(baseEnemy *enemy){
    enemies.removeOne(enemy);

    if(!enemy->getKilled()){
        life--;
        emit updateUI(UIupdate::life);
        //TODO more substraction based on enemy
    }

    enemy->deleteLater();
}

void game::wave(){
    wave_stat++;
    emit updateUI(UIupdate::wave);
}

qreal game::getLife(){
    return life;
}

qreal game::getMoney(){
    return money;
}

qreal game::getWave(){
    return wave_stat;
}
