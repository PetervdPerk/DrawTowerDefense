#include "gameui.h"
#include <QDebug>

gameUI::gameUI(View* gameView,game* game_obj,QObject *parent) :
    QObject(parent)
{
    this->gameView = gameView;
    this->game_obj = game_obj;

    waveText.setDefaultTextColor(Qt::white);
    lifeText.setDefaultTextColor(Qt::white);
    moneyText.setDefaultTextColor(Qt::white);
    waveTimerText.setDefaultTextColor(Qt::white);
    //TODO show status of time before next wave


    update(UIupdate::all);

    waveText.setPos(20,0);
    moneyText.setPos(20,20);
    lifeText.setPos(20,40);
 //   waveTimerText.setPos(0,60);

    gameView->addItem(&waveText);
    gameView->addItem(&moneyText);
    gameView->addItem(&lifeText);
 //   gameView->addItem(&waveTimerText);

}

void gameUI::update(UIupdate upd){

    switch(upd) {

    case UIupdate::life:
        lifeText.setPlainText(QString("life: %1").arg(game_obj->getLife()));
        break;

    case UIupdate::money:
        moneyText.setPlainText(QString("Money: %1").arg(game_obj->getMoney()));
        break;

    case UIupdate::wave:
        waveText.setPlainText(QString("Wave: %1").arg(game_obj->getWave()));
        break;

 //   case UIupdate::waveTimer:
 //       waveText.setPlainText(QString("Wave: %1").arg(game_obj->getWave()));
 //       break;

    case UIupdate::all:
        lifeText.setPlainText(QString("life: %1").arg(game_obj->getLife()));
        waveText.setPlainText(QString("Wave: %1").arg(game_obj->getWave()));
        moneyText.setPlainText(QString("Money: %1").arg(game_obj->getMoney()));
        break;

    }

}
