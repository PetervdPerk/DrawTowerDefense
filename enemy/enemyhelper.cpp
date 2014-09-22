#include "enemyhelper.h"

#define SPEEDFACTOR 15

enemyHelper::enemyHelper(QPainterPath *gamePath, QObject *parent) :
    QObject(parent)
{
    this->gamePath = gamePath;

    for( double i = 0 ; i < gamePath->length(); i++) {
        positions.push_back(QVariantAnimation::KeyValue(i/gamePath->length(),
                                 gamePath->pointAtPercent(gamePath->percentAtLength(i))));
    }
}

QVariantAnimation::KeyValues* enemyHelper::getPositions(){
    return &positions;
}

qreal enemyHelper::getDurationOfPath(){
    return gamePath->length() * SPEEDFACTOR;
}
