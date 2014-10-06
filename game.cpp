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

    //QObject::connect(wavesMgr) (times between waves)
    QObject::connect(&moneyTimer, SIGNAL(timeout()), this, SLOT(UpdateMoney()));
    moneyTimer.start(6000);

    addTower();

    addLaserTower();
}

void game::updateTower(QPointF loc, int id){
    towers.at(id)->setCenterPos(loc);
}


void game::addTower(qreal x, qreal y) {
    baseTower* tower = new baseTower(x,y,&enemies);
    tower->setFlag(QGraphicsItem::ItemIsMovable, true);
    towers.append(tower);
    gameView->addItem(tower);
}

void game::addLaserTower(qreal x, qreal y) {
    laserTower* tower2 = new laserTower(x,y,&enemies);
    tower2->setFlag(QGraphicsItem::ItemIsMovable, true);
    towers.append(tower2);
    gameView->addItem(tower2);
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

void game::UpdateMoney(){
    money += 250; //add money after every wave
    emit updateUI(UIupdate::money);
}

void game::CheckTowers(){
  /*
   * this function should check the amount of towers placed
   * and if a tower is placed it should substract the price of that tower from the current money
   * it should also check if it is possible to place a new tower (enough money).
   *

 gameView->customEvent();
    if(towers->getPrice > getMoney())
    {
        //qdebug << "cant place tower";
    }
    */
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
