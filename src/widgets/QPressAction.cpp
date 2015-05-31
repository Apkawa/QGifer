//
// Created by apkawa on 31.05.15.
//

#include "QPressAction.h"

bool QPressAction::eventFilter(QObject *obj, QEvent *event) {
    if (
            event->type() != QEvent::Type::MouseButtonPress &&
            event->type() != QEvent::Type::MouseButtonRelease
            ) {
        return QAction::eventFilter(obj, event);
    }
    QString className = QString(obj->metaObject()->className());
    if (className == "QToolButton") {
        QToolButton *actionToolButton = qobject_cast<QToolButton *>(obj);
        QList<QAction *> actionList = (*actionToolButton).actions();
        if (!actionList.empty()) {
            QAction *action = qobject_cast<QAction *>(actionList.first());
            if (action->isEnabled()) {
                if (event->type() == QEvent::Type::MouseButtonPress) {
                    is_long_press = false;
                    pressTimer->start(500);
                }

                if (event->type() == QEvent::Type::MouseButtonRelease) {
                    pressTimer->stop();
                    if (is_long_press) {
                        emit press_end();
                        is_long_press = false;
                        return false;
                    }
                }
            }

        }

    }
    return QAction::eventFilter(obj, event);
}

void QPressAction::press_timeout() {
    is_long_press = true;
    pressTimer->stop();
    emit press_start();


}
