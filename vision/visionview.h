#ifndef VISIONVIEW_H
#define VISIONVIEW_H

#include <QDialog>
#include <QImage>

namespace Ui {
class visionView;
}


namespace vision {
class visionView : public QDialog
{
    Q_OBJECT

public:
    explicit visionView(QWidget *parent = 0);
    ~visionView();

public slots:
    void updateFrame(const QImage &frame);

private:
    Ui::visionView *ui;
};
}

#endif // VISIONVIEW_H
