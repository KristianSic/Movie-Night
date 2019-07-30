#ifndef MYSLIDER_H
#define MYSLIDER_H

#include <QSlider>
#include <QMouseEvent>
#include <QStyleOptionSlider>
#include <QRect>

class MySlider : public QSlider
{
    Q_OBJECT
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

public:
     MySlider(QWidget *parent = nullptr);
     ~MySlider();
signals:
    void clicked(int value);
};

#endif // MYSLIDER_H
