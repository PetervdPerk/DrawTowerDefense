#ifndef ENEMYANIMATION_H
#define ENEMYANIMATION_H

#include <QPropertyAnimation>

class enemyAnimation : public QPropertyAnimation
{
public:
    enemyAnimation(QObject *target, const QByteArray &propertyName, QObject *parent = 0);
    void setCurrentValue(qreal currValue);
};

#endif // ENEMYANIMATION_H
