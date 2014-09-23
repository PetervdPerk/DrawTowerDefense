#ifndef GAMEUI_H
#define GAMEUI_H

#include <QObject>
#include <QGraphicsTextItem>
#include "gameUIenum.h"
#include "game.h"

class gameUI : public QObject
{
    Q_OBJECT
public:
    gameUI(View* gameView, game* game_obj,QObject *parent = 0);

signals:

public slots:
    void update(UIupdate upd);

private:
    View* gameView;
    game* game_obj;

    QGraphicsTextItem waveText;
    QGraphicsTextItem lifeText;
    QGraphicsTextItem moneyText;

};

#endif // GAMEUI_H
