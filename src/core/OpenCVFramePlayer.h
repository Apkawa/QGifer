//
// Created by apkawa on 12/14/15.
//

#ifndef QGIFER_OPENCVFRAMEPLAYER_H
#define QGIFER_OPENCVFRAMEPLAYER_H

#include <QtCore/QDebug>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "AbstractFramePlayer.h"

#define SLOW_SEEK_OFFSET 55

namespace core {

    class OpenCVFramePlayer : public core::AbstractFramePlayer {

    public:

        OpenCVFramePlayer(const QString &filepath) : AbstractFramePlayer(filepath) {
            setSource(filepath);
        }

        virtual void seek(u_long pos) override;

        virtual bool setSource(const QString &src) override;

        virtual void setPos(u_long pos) override;

        virtual double getFPS() override;
    private:
        cv::VideoCapture vcap;
        bool raw;

    public:
        virtual QString getCodecName() override;

        virtual bool isOpened() override;

        virtual void close() override;

        bool isRaw() {
            return raw;
        }

        void slowSetPos(u_long pos);

        QImage nextFrame();

        virtual QImage prevFrame() override;
    };

}
#endif //QGIFER_OPENCVFRAMEPLAYER_H
