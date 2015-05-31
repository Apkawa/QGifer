//
// Created by apkawa on 31.05.15.
//

#ifndef QGIFER_QJUMPSLIDER_H
#define QGIFER_QJUMPSLIDER_H


#include <QtGui/QSlider>
#include <QtGui/QMouseEvent>
#include <QtGui/QWidget>


class QJumpSlider: public QSlider
{
Q_OBJECT

public:
    explicit QJumpSlider(QWidget *parent = 0): QSlider(parent){};
    explicit QJumpSlider(Qt::Orientation orientation, QWidget *parent = 0): QSlider(orientation, parent) {};

protected:
    void mousePressEvent( QMouseEvent * event );


};



#endif //QGIFER_QJUMPSLIDER_H
