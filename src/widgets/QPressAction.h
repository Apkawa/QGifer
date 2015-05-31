//
// Created by apkawa on 31.05.15.
//

#ifndef QGIFER_QPRESSACTION_H
#define QGIFER_QPRESSACTION_H

#include <QtCore/QObject>
#include <QtCore/QDebug>
#include <QtCore/QEvent>
#include <QtCore/QTimer>

#include <QtGui/QAction>
#include <QtGui/QMouseEvent>
#include <QtGui/QToolButton>


class QPressAction: public QAction
{
Q_OBJECT

public:
    explicit QPressAction(QObject* parent): QAction(parent) {
        pressTimer = new QTimer(this);
        connect(pressTimer, SIGNAL(timeout()), this, SLOT(press_timeout()));
        connect(this, SIGNAL(press_start()), this, SLOT(press_start_event()));
        connect(this, SIGNAL(triggered()), this, SLOT(trigger_event()));
        connect(this, SIGNAL(press_end()), this, SLOT(press_end_event()));

    };

signals:
    void press_start();
    void press_end();
    void press(bool start);

protected:
    QTimer * pressTimer;
    bool is_long_press = false;

    bool eventFilter(QObject * obj, QEvent * event);


private slots:

    void press_timeout();

    void trigger_event() {
        qDebug() << this << "triggered";
    }

    void press_start_event() {
        qDebug() << this << "press_start event";
    }

    void press_end_event() {
        qDebug() << this << "press_end event";

    }

};


#endif //QGIFER_QPRESSACTION_H
