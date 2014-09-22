#ifndef GAMEUI_H
#define GAMEUI_H

#include <QObject>
#include "game.h"

class gameUI : public QObject
{
    Q_OBJECT
public:
    gameUI(View* gameView, game* game_obj,QObject *parent = 0);

signals:

public slots:
    void update();

private:
    View* gameView;
    game* game_obj;

};

#endif // GAMEUI_H
