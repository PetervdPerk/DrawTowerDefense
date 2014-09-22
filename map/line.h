#ifndef LINE_H
#define LINE_H

#include <QObject>
#include <QPainterPath>

class Line : public QObject
{
    Q_OBJECT
public:
    explicit Line(QObject *parent = 0);

signals:

public slots:

private:
    QPainterPath* path;


};

#endif // LINE_H
