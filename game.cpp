#include "game.h"

game::game(View* gameView, QObject *parent) :
    QObject(parent)
{
    this->gameView = gameView;
    //TODO DRAW WELCOME screen


}

void game::startDraw(){
    gameView->setBackground(QColor("Black"));
}

void game::drawLine(QPolygonF line){
    if(paintOnce){
        gameView->setBackground(QColor("White"));
        path.addPolygon(line.mid(0,line.length()/2));

        QGraphicsItem* graphicsLine = new GameLine(line); //Filled line

        gameView->addItem(graphicsLine);

        enemyHelp = new enemyHelper(&path);
        wavesMgr = new waveManager(&enemies,gameView,enemyHelp,this);

        QObject::connect(wavesMgr, SIGNAL(newWave()), this, SLOT(wave()));
        QObject::connect(wavesMgr, SIGNAL(enemyDown(baseEnemy*)), this, SLOT(enemyDown(baseEnemy*)));

        //QObject::connect(wavesMgr) (times between waves)
        QObject::connect(&moneyTimer, SIGNAL(timeout()), this, SLOT(UpdateMoney()));
        moneyTimer.start(6000);

        gameStarted = true;
        paintOnce = false;
    }
}

void game::glyphLoc(QPointF loc, int id){
    if(gameStarted){
        if(towers.contains(id)){
            towers[id]->setCenterPos(loc);
        } else {
            if(money >= 2000){
                addTower(id);
                towers[id]->setCenterPos(loc);
                money = money - 2000;
                emit updateUI(UIupdate::money);
            }
        }
    }else{
        //detect start / stop glyph
    }
}


void game::addTower(int id) {
    baseTower* tower = new baseTower(0,0,&enemies);
    tower->setFlag(QGraphicsItem::ItemIsMovable, true);
    towers.insert(id,tower);
    gameView->addItem(tower);
}

void game::addLaserTower(int id) {
    laserTower* tower2 = new laserTower(0,0,&enemies);
    tower2->setFlag(QGraphicsItem::ItemIsMovable, true);
    towers.insert(id,tower2);
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

qreal game::getLife(){
    return life;
}

qreal game::getMoney(){
    return money;
}

qreal game::getWave(){
    return wave_stat;
}
