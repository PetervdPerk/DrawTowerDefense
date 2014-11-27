#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "view.h"
#include "game.h"
#include "gameui.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void resizeEvent(QResizeEvent *);
    void addLayoutToVBOX(QLayout *layout);
    void addLayoutToVBOX2(QLayout *layout);

public slots:
    void updateFrame(const QImage &frame);

private slots:
    void on_tabWidget_currentChanged(int index);

private:
    Ui::MainWindow *ui;
    View* gameView;
    game *gm;
    gameUI *gui;
};

#endif // MAINWINDOW_H
