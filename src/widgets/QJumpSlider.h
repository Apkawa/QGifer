//
// Created by apkawa on 31.05.15.
//

#ifndef QGIFER_QJUMPSLIDER_H
#define QGIFER_QJUMPSLIDER_H

#include <QtCore/QDebug>
#include <QtCore/QTime>

#include <QtGui/QSlider>
#include <QtGui/QMouseEvent>
#include <QtGui/QWidget>

class QJumpSlider : public QSlider {
Q_OBJECT

public:
    explicit QJumpSlider(QWidget *parent = 0) : QSlider(parent) {
        this->installEventFilter(this);
        this->setMouseTracking(true);
    };

    explicit QJumpSlider(Qt::Orientation orientation, QWidget *parent = 0) : QSlider(orientation, parent) { };


protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    double video_fps = 0;

public slots:

    void setVideoFps(double video_fps) {
        this->video_fps = video_fps;
    };

};


#endif //QGIFER_QJUMPSLIDER_H
