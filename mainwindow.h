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

private:
    Ui::MainWindow *ui;
    View* gameView;
    game *gm;
    gameUI *gui;
};

#endif // MAINWINDOW_H
