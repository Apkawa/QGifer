//
// Created by apkawa on 12/14/15.
//

#include "OpenCVFramePlayer.h"

using namespace core;

bool OpenCVFramePlayer::setSource(const QString &src) {
    if (vcap.isOpened())
        vcap.release();
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

    interval = estimateInterval();

    if (!interval) {
        interval = 40;
    }

    return false;
}

void OpenCVFramePlayer::setPos(u_long pos) {

}

double OpenCVFramePlayer::getFPS() {
    return AbstractFramePlayer::getFPS();
}

void OpenCVFramePlayer::setFPS() {

}

Size OpenCVFramePlayer::getOriginalSize() {
    return AbstractFramePlayer::getOriginalSize();
}

QString OpenCVFramePlayer::getCodecName() {
    if (!vcap.isOpened())
        return "";
    int ex = static_cast<int>(vcap.get(CV_CAP_PROP_FOURCC));
    char EXT[] = {(char) (ex & 0XFF), (char) ((ex & 0XFF00) >> 8), (char) ((ex & 0XFF0000) >> 16),
                  (char) ((ex & 0XFF000000) >> 24), 0};
    return QString(EXT);
}

QImage OpenCVFramePlayer::getFrame() {
    if (isOpened() && frame != NULL) {
        Mat m;
        vcap >> m;
        currentPos++;
        if (currentPos >= totalFrames) {
            currentPos = totalFrames - 1;
        }

        qDebug() << "current pos: " << currentPos << "/" << totalFrames;
        currentFrame = QImage((uchar *) m.data, m.cols, m.rows, m.step,
                              QImage::Format_RGB888).rgbSwapped();
    }
}

bool OpenCVFramePlayer::isOpened() {
    return vcap.isOpened();
}

void OpenCVFramePlayer::close() {
    vcap.release();
}
