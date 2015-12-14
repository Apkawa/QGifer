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


#include "frameplayer.h"

FramePlayer::FramePlayer(QWidget *parent) : QWidget(parent), player(NULL),
                                            timerId(-1), status(Stopped),
                                            statusbar(NULL),
                                            medianblur(0) {
    setupUi(this);
    workspace = new Workspace(frame);
    workspace->enableBackground(true);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(seek(int)));

}

FramePlayer::~FramePlayer() {
    //delete workspace;
}

bool FramePlayer::openSource(const QString &src) {
    player = core::OpenCVFramePlayer(src);
    killTimer(timerId);
    timerId = -1;
    if (!player.isOpened()) {
        qDebug() << "FramePlayer::openSource cannot load video: " << src;
        close();
        return false;
    }

    qDebug() << "FramePlayer::openSource new video loaded: " << src;

    filepath = src;
    interval = player.estimateInterval();

    slider->setMaximum(countFrames() - 1);
    slider->setValue(0);
    slider->setVideoFps(player.getFPS());
    nextFrame();
    return true;
}

void FramePlayer::close() {
    stop();
    player.close();
    //fakeFrames += frames;
    filepath.clear();
    slider->setValue(0);
    slider->setVideoFps(0.0);
    showDefaultScreen();
    updateStatus(status);
}

void FramePlayer::nextFrame() {
    if (!player.hasNextFrame()) {
        return;
    }
    player.nextFrame();

    workspace->setImage(player.getFrame(), frame->size());

    workspace->updateFrameIndex(player.getPos());
    emit frameChanged(player.getPos());
    updateSlider(player.getPos());
    updateStatus(timerId == -1 ? Stopped : Playing);
}

void FramePlayer::reverse_play() {
    // todo optimize
    qDebug() << "PLAY reversed:";
    is_reverse_play = true;
    if (!countFrames()) {
        return;
    }
    if (timerId == -1) {
        timerId = startTimer(interval);
    }
    updateStatus(Playing);
}

void FramePlayer::play() {
    qDebug() << "PLAY";
    is_reverse_play = false;
    if (!countFrames()) {
        return;
    }

    if (player.getPos() == countFrames() - 1) {
        qDebug() << "currentPos:" << player.getPos() << " countFrames():" << countFrames();
        seek(0);
    }

    if (timerId == -1) {
        timerId = startTimer(player.estimateInterval());
    }
    updateStatus(Playing);
}

void FramePlayer::stop() {
    pause();
    seek(0);
}

void FramePlayer::pause() {
    killTimer(timerId);
    timerId = -1;
    updateStatus(Stopped);
}

void FramePlayer::seek(int pos) {
    qDebug() << "seeking pos: " << pos;
    if (status != Playing) {
        player.seek(pos);
    } else {
        player.setPos(pos);
    }
}

void FramePlayer::timerEvent(QTimerEvent *) {
    if (is_reverse_play) {
        prevFrame();
    } else {
        nextFrame();
    }
    if ((!player.hasNextFrame()) || (!player.hasPrevFrame())) {
        stop();
    }
}


void FramePlayer::updateSlider(int pos) {
    if (pos < 0) {
        pos = 0;
    }
    disconnect(slider, SIGNAL(valueChanged(int)), this, SLOT(seek(int)));
    slider->setValue(pos);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(seek(int)));
}

void FramePlayer::updateStatus(Status s) {
    QString info = "";
    status = s;
    info = (status == Stopped ? "Stopped" : "Playing");

    //slider->setEnabled(status == Stopped);

    long total = countFrames();
    if (total)
        info += ", frame: " + QString::number(player.getPos()) + "/" + QString::number(total - 1) +
                " (" + QString::number(total) + " in total)";
    else
        info += ", no frames";
    slider->setMaximum(total == 0 ? 0 : total - 1);
    if (statusbar) {
        statusbar->showMessage(info);
    }

    emit statusUpdated(s);
}


void FramePlayer::resizeEvent(QResizeEvent *) {
    //qDebug() << "player resize event";
    workspace->updateBackground();
    if (countFrames()) {
        workspace->setImage(player.getFrame(), frame->size());
    }
    else {
        showDefaultScreen();

    }
    workspace->setFixedSize(frame->size());
}

void FramePlayer::setStatusBar(QStatusBar *sb) {
    statusbar = sb;
    //statusLabel->setVisible(sb==NULL);
}


void FramePlayer::renderDefaultTextImage(const QString &text) {
    QPixmap def(800, 450);
    def.fill(Qt::transparent);
    defaultImg = def.toImage();
    QFont f;
    f.setPointSize(26);
    f.setBold(true);
    QImage txt = TextRenderer::renderText(text, f, QColor(30, 30, 30), QColor(255, 255, 255), 1).
            scaledToWidth(defaultImg.width(), Qt::SmoothTransformation);
    QPainter p(&defaultImg);
    p.drawImage((defaultImg.width() - txt.width()) / 2,
                (defaultImg.height() - txt.height()) / 2, txt);
}


core::Size FramePlayer::getOriginalSize() const {
    return player.getOriginalSize();
}



