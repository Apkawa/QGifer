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

FramePlayer::FramePlayer(QWidget *parent) : QWidget(parent), totalFrames(0), originalSize(Size(0, 0)),
                                            currentPos(-1), timerId(-1), status(Stopped), statusbar(NULL),
                                            raw(false), interval(40), medianblur(0) {
    setupUi(this);
    workspace = new Workspace(frame);
    workspace->enableBackground(true);
    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(seek(int)));
}

FramePlayer::~FramePlayer() {
    //delete workspace;
}

bool FramePlayer::openSource(const QString &src) {
    if (vcap.isOpened())
        vcap.release();
    vcap.open(src.toStdString());
    originalSize.width = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
    originalSize.height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
    killTimer(timerId);
    timerId = -1;
    currentPos = -1;
    if (!vcap.isOpened()) {
        qDebug() << "FramePlayer::openSource cannot load video: " << src;
        close();
        return false;
    }

    qDebug() << "FramePlayer::openSource new video loaded: " << src;

#if defined(Q_WS_X11)
    QString codec = codecName();
    raw = (codec == "MJPG" || codec == "I420" || codec == "YUV4");
    qDebug() << "codec name: " << codecName();
#endif

    filepath = src;
    totalFrames = vcap.get(CV_CAP_PROP_FRAME_COUNT);
    qDebug() << "total frames: " << totalFrames;

    interval = estimateInterval();
    if (!interval)
        interval = 40;

    slider->setMaximum(countFrames() - 1);

    slider->setValue(0);
    nextFrame();
    return true;
}

void FramePlayer::close() {
    stop();
    vcap.release();
    //fakeFrames += frames;
    totalFrames = 0;
    filepath.clear();
    originalSize = Size(0, 0);
    slider->setValue(0);
    showDefaultScreen();
    updateStatus(status);
}

void FramePlayer::nextFrame() {
    //qDebug() << "FramePlayer::nextFrame(): frames: " << frames << ", fakeFrames: " << fakeFrames << ", currentPos: " << currentPos;
    if (!vcap.isOpened())
        return;

    if (vcap.isOpened() && currentPos + 1 <= totalFrames - 1) {
        //qDebug() << "next REAL frame...";
        Mat m;
        //vcap.grab();
        //vcap.retrieve(m);

        vcap >> m;

        //currentPos = vcap.get(CV_CAP_PROP_POS_FRAMES); //videocapture zlicza roznie przy roznych kodekach
        currentPos++;
        if (currentPos >= totalFrames) {
            currentPos = totalFrames - 1;
        }
        //qDebug() << "current pos: " << currentPos << "/" << frames;
        //cvtColor(m,m,CV_BGR2RGB);
        if (medianblur) {
            medianBlur(m, m, medianblur % 2 ? medianblur : medianblur + 1);
        }
        currentFrame = QImage((uchar *) m.data, m.cols, m.rows, m.step,
                              QImage::Format_RGB888).rgbSwapped();
    }
    else {
        return;
    }

    workspace->setImage(currentFrame, frame->size());
    workspace->updateFrameIndex(currentPos);
    emit frameChanged(currentPos);
    updateSlider(currentPos);

    updateStatus(timerId == -1 ? Stopped : Playing);
}

void FramePlayer::play() {
    if (!countFrames())
        return;

    if (currentPos == countFrames() - 1)
        seek(0);

    if (timerId == -1)
        timerId = startTimer(interval);
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

void FramePlayer::setPos(long pos) {
    if (!vcap.isOpened())
        return;

    if (pos < 0) {
        pos = 0;
    }
    if (pos >= totalFrames) {
        pos = totalFrames - 1;
    }
    currentPos = pos;
    vcap.set(CV_CAP_PROP_POS_FRAMES, currentPos);
    currentPos--;
    nextFrame();
}

void FramePlayer::seek(int pos) {
    qDebug() << "seeking pos: " << pos;
    setPos(totalFrames);
}

void FramePlayer::timerEvent(QTimerEvent *) {
    //qDebug() << "tick";
    nextFrame();
    if (currentPos >= countFrames() - 1){

        stop();
    }
}


void FramePlayer::updateSlider(int pos) {
    if (pos < 0) pos = 0;
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
        info += ", frame: " + QString::number(currentPos) + "/" + QString::number(total - 1) +
                " (" + QString::number(total) + " in total)";
    else
        info += ", no frames";
    slider->setMaximum(total == 0 ? 0 : total - 1);
    if (statusbar)
        statusbar->showMessage(info);

    emit statusUpdated(s);
}


void FramePlayer::resizeEvent(QResizeEvent *) {
    //qDebug() << "player resize event";
    workspace->updateBackground();
    if (countFrames())
        workspace->setImage(currentFrame, frame->size());
    else
        showDefaultScreen();
    workspace->setFixedSize(frame->size());
}

void FramePlayer::setStatusBar(QStatusBar *sb) {
    statusbar = sb;
    //statusLabel->setVisible(sb==NULL);
}

QString FramePlayer::codecName() {
    if (!vcap.isOpened())
        return "";
    int ex = static_cast<int>(vcap.get(CV_CAP_PROP_FOURCC));
    char EXT[] = {(char) (ex & 0XFF), (char) ((ex & 0XFF00) >> 8), (char) ((ex & 0XFF0000) >> 16),
                  (char) ((ex & 0XFF000000) >> 24), 0};
    return QString(EXT);
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
