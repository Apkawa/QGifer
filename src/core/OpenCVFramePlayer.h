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

namespace core {

    class OpenCVFramePlayer : public core::AbstractFramePlayer {

    public:
        virtual bool setSource(const QString &src) override;

        virtual void setPos(u_long pos) override;

        virtual double getFPS() override;

        virtual void setFPS() override;

        virtual core::Size getOriginalSize() override;

        OpenCVFramePlayer(const QString &filepath) : AbstractFramePlayer(filepath) { }

    private:
        cv::VideoCapture vcap;
        bool raw;

    public:
        virtual QString getCodecName() override;

        virtual QImage getFrame() override;

        virtual bool isOpened() override;

        virtual void close() override;

        bool isRaw() {
            return raw;
        }
    };

}
#endif //QGIFER_OPENCVFRAMEPLAYER_H
