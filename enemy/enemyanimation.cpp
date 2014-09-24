#include "enemyanimation.h"

enemyAnimation::enemyAnimation(QObject *target, const QByteArray &propertyName, QObject *parent)
    : QPropertyAnimation(target, propertyName, parent)
{
}

void enemyAnimation::setCurrentValue(qreal currValue){
    //QPropertyAnimation::updateCurrentValue(currValue);
    //const qreal progress = QVariantAnimation::easingCurve().valueForProgress( qreal(currentTime()) / qreal(duration()));
    QVariant val = currentValue();
    setDuration(currValue);
    QVariantAnimation::updateCurrentValue(val);
    //BUG
}
