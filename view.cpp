#include "view.h"
#include <QDebug>

View::View(QObject *parent, QGraphicsView *view, qreal width, qreal height) :
    QObject(parent)
{
    this->view = view;
    Viewheight = height;
    Viewwidth = width;

    //view->setViewport(new QGLWidget(QGLFormat(QGL::SampleBuffers))); //GPU rendering :)

    scene = new QGraphicsScene(this);
    scene->setSceneRect( 0, 0, 800, 640 );
    view->setScene(scene);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),scene, SLOT(advance()));
    timer->start(200);

    view->setRenderHints( QPainter::Antialiasing );

    resizeHandler();
}

void View::addItem(QGraphicsItem * item){
    scene->addItem(item);
}

void View::removeItem(QGraphicsItem * item){
    scene->removeItem(item);
}


void View::resizeHandler(){
    view->fitInView( scene->sceneRect(), Qt::KeepAspectRatio );
}

void View::resizeView(qreal width, qreal height){
    Viewwidth = width;
    Viewheight = height;
    scene->setSceneRect( 0.0, 0.0, width, height );
    rectangleView->setRect(0,0,width,height);
    resizeHandler();
}

qreal View::getViewHeight(){
    return Viewheight;
}

qreal View::getViewWidth(){
    return Viewwidth;
}
