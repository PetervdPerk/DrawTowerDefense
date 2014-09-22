#ifndef View_H
#define View_H

#include <QObject>
#include <QGraphicsEllipseItem>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>
#include <QHash>
#include <QPen>
#include <QGLWidget>
#include <QTimer>

class View : public QObject
{
    Q_OBJECT
public:
    explicit View(QObject *parent, QGraphicsView *view, qreal width, qreal height);
    void resizeHandler();
    void resizeView(qreal width, qreal height);
    void resizeDevices(qreal size);

    qreal getViewHeight();
    qreal getViewWidth();

    void addItem(QGraphicsItem * item);
    void removeItem(QGraphicsItem * item);

signals:

public slots:

private:
    qreal ellipseSize = 50;
    qreal Viewheight;
    qreal Viewwidth;

    QPen outlinePen;

    QGraphicsView *view; //GraphicsView

    QGraphicsScene *scene;
    QGraphicsEllipseItem *ellipse;
    QGraphicsRectItem *rectangleView;

    QGraphicsSceneMouseEvent mouse_event;

    QTimer *timer;

};

#endif // View_H
