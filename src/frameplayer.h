/************************************************************************
** QGifer
** Copyright (C) 2013 Lukasz Chodyla <chodak166@op.pl>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
************************************************************************/

#ifndef FRAMEPLAYER_H
#define FRAMEPLAYER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QWidget>
#include <QImage>
#include <QStatusBar>
#include <QPixmap>
#include <QMessageBox>
#include <core/OpenCVFramePlayer.h>

#include "textrenderer.h"
#include "workspace.h"
#include "ui_frameplayer.h"

using namespace cv;

class FramePlayer : public QWidget, public Ui::FramePlayerForm {
Q_OBJECT;

public:
    enum Status {
        Stopped, Playing
    };

    FramePlayer(QWidget *parent);

    virtual ~FramePlayer();

    bool openSource(const QString &src);

    long countFrames() const {
        return player.getTotalFrames();
    }

    const long getCurrentPos() const {
        return player.getPos();
    }

    QImage *getCurrentFrame() {
        return player.getPointerFrame();
    }


    double fps() {
        return player.getFPS();
    }

    bool isOpened() {
        return player.isOpened();
    }

    void setStatusBar(QStatusBar *sb);

    Status getStatus() const {
        return status;
    }

    const QString &source() const {
        return filepath;
    }

    void showDefaultScreen() {
//        currentFrame = defaultImg;
        workspace->setImage(defaultImg, frame->size(), true, true);
    }

    Workspace *getWorkspace() {
        return workspace;
    }

    void setMedianBlur(int m) {
        medianblur = m;
    }

    void renderDefaultTextImage(const QString &text);

    core::Size getOriginalSize() const;


private:
    core::OpenCVFramePlayer player;
    int medianblur;
    QString filepath;
    Status status;
    QImage defaultImg;
    Workspace *workspace;
    int interval;
    int timerId;
    QStatusBar *statusbar;
    bool is_reverse_play = false;

    void timerEvent(QTimerEvent *);

    void updateSlider(int pos);

    void updateStatus(Status s);

    void resizeEvent(QResizeEvent *);


public slots:

    void press_start_event() {
        qDebug() << this << "press_start event";
    }

    void press_end_event() {
        qDebug() << this << "press_end event";

    }

    void nextFrame();

    void prevFrame() {
        player.prevFrame();
    }

    void reverse_play();

    void play();

    void stop();

    void pause();

    void seek(int pos);

    void close();

    void setInterval(int i) {
        interval = i;
        if (status == Playing) {
            pause();
            play();
        }
    }

    void setFps(double f) {
        setInterval(1000 / f);
    }

signals:

    void frameChanged(long);

    void statusUpdated(FramePlayer::Status status);

};

#endif
