//
// Created by apkawa on 12/14/15.
//

#include "OpenCVFramePlayer.h"

using namespace core;

bool OpenCVFramePlayer::setSource(const QString &src) {
    if (vcap.isOpened()) {
        vcap.release();
    }
    vcap.open(src.toStdString());
    frameSize.width = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
    frameSize.height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);

    position = 0;
    if (!vcap.isOpened()) {
        qDebug() << "FramePlayer::openSource cannot load video: " << src;
        close();
        return false;
    }

    qDebug() << "FramePlayer::openSource new video loaded: " << src;

#if defined(Q_WS_X11)
    QString codec = getCodecName();
    raw = (codec == "MJPG" || codec == "I420" || codec == "YUV4");
    qDebug() << "codec name: " << getCodecName();
#endif

    filepath = src;
    totalFrames = vcap.get(CV_CAP_PROP_FRAME_COUNT);
    qDebug() << "total frames: " << totalFrames;
    return true;
}

void OpenCVFramePlayer::setPos(u_long pos) {
    if (!isOpened()) {
        return;
    }
    if (pos >= totalFrames) {
        pos = totalFrames - 1;
    }
    position = pos;

    vcap.set(CV_CAP_PROP_POS_FRAMES, position);
    position--;
    nextFrame();
}

void OpenCVFramePlayer::slowSetPos(u_long pos) {

    if (!isOpened()) {
        return;
    }

    if (pos < 0) {
        pos = 0;
    }
    if (pos >= totalFrames) {
        pos = totalFrames - 1;
    }
    position = pos;

    long startPos = position - SLOW_SEEK_OFFSET;
    if (startPos < 0) {
        startPos = 0;
    }

    vcap.set(CV_CAP_PROP_POS_FRAMES, startPos);
    for (; startPos < position; startPos++) {
        vcap.grab();
    }
    position--;
    nextFrame();
}

double OpenCVFramePlayer::getFPS() {
    if (isOpened()) {
        return vcap.get(CV_CAP_PROP_FPS);
    }
    return 0;
}

QString OpenCVFramePlayer::getCodecName() {
    if (!isOpened()) {
        return "";
    }
    int ex = static_cast<int>(vcap.get(CV_CAP_PROP_FOURCC));
    char EXT[] = {(char) (ex & 0XFF),
                  (char) ((ex & 0XFF00) >> 8),
                  (char) ((ex & 0XFF0000) >> 16),
                  (char) ((ex & 0XFF000000) >> 24),
                  0};
    return QString(EXT);
}

QImage OpenCVFramePlayer::prevFrame() {
    seek(position - 1);
    return getFrame();
}

QImage OpenCVFramePlayer::nextFrame() {
    if (isOpened()) {
        cv::Mat m;
        vcap >> m;
        position++;
        if (position >= totalFrames) {
            position = totalFrames - 1;
        }

        qDebug() << "current pos: " << position << "/" << totalFrames;
        frame = QImage(m.data, m.cols, m.rows, m.step,
                       QImage::Format_RGB888).rgbSwapped();
    }
    return getFrame();
}

bool OpenCVFramePlayer::isOpened() {
    return vcap.isOpened();
}

void OpenCVFramePlayer::close() {
    vcap.release();
}

void OpenCVFramePlayer::seek(u_long pos) {
    if (pos > 0 && (position - pos) < SLOW_SEEK_OFFSET) {
        slowSetPos(pos);
    } else {
        setPos(pos);
    }
}
