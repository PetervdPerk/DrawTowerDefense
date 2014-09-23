#include "gameui.h"
#include <QDebug>

gameUI::gameUI(View* gameView,game* game_obj,QObject *parent) :
    QObject(parent)
{
    this->gameView = gameView;
    this->game_obj = game_obj;
    //TODO show status of waves, timer, health, money

    update(UIupdate::all);

    moneyText.setPos(0,20);
    lifeText.setPos(0,40);

    gameView->addItem(&waveText);
    gameView->addItem(&moneyText);
    gameView->addItem(&lifeText);

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

    case UIupdate::all:
        lifeText.setPlainText(QString("life: %1").arg(game_obj->getLife()));
        waveText.setPlainText(QString("Wave: %1").arg(game_obj->getWave()));
        moneyText.setPlainText(QString("Money: %1").arg(game_obj->getMoney()));
        break;

    }

}
