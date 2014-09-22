#include "gameui.h"

gameUI::gameUI(View* gameView,game* game_obj,QObject *parent) :
    QObject(parent)
{
    this->gameView = gameView;
    this->game_obj = game_obj;
}

void gameUI::update(){

}
