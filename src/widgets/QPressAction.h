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


class QPressAction : public QAction {
Q_OBJECT

public:
    explicit QPressAction(QObject *parent) : QAction(parent) {
        pressTimer = new QTimer(this);
        connect(pressTimer, SIGNAL(timeout()), this, SLOT(press_timeout()));

    };

signals:

    void press_start();

    void press_end();

    void press(bool start);

protected:

    bool eventFilter(QObject *obj, QEvent *event);

private:
    QTimer *pressTimer;
    bool is_long_press = false;

private slots:

    void press_timeout();
};


#endif //QGIFER_QPRESSACTION_H
