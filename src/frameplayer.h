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
        return totalFrames;
    }

    const long &getCurrentPos() const {
        return currentPos;
    }

    QImage *getCurrentFrame() {
        return &currentFrame;
    }

    int estimateInterval() {
        return vcap.isOpened() ? round(1000.0 / vcap.get(CV_CAP_PROP_FPS)) : 0;
    }

    double fps() {
        return vcap.isOpened() ? vcap.get(CV_CAP_PROP_FPS) : 0;
    }

    void setStatusBar(QStatusBar *sb);

    void setPos(long pos);

    void slowSetPos(long pos);

    Status getStatus() const {
        return status;
    }

    const QString &source() const {
        return filepath;
    }

    void setDefaultImage(const QImage &img) {
        defaultImg = img.copy();
    }

    void enableAntialiasing(bool enable) {
        workspace->enableAntialiasing(enable);
        workspace->setImage(currentFrame, frame->size());
    }

    QString codecName();

    void showDefaultScreen() {
        currentFrame = defaultImg;
        workspace->setImage(defaultImg, frame->size(), true, true);
    }

    Workspace *getWorkspace() {
        return workspace;
    }

    void setMedianBlur(int m) {
        medianblur = m;
    }

    void renderDefaultTextImage(const QString &text);

private:
    VideoCapture vcap;
    int medianblur;
    QString filepath;
    Status status;
    Size originalSize;
    QImage currentFrame;
    QImage defaultImg;
    Workspace *workspace;
    long totalFrames;
    long currentPos;
    int interval;
    int timerId;
    QStatusBar *statusbar;
    bool raw;
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
        seek(currentPos - 1);
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
