//
// Created by apkawa on 12/13/15.
//

#ifndef QGIFER_FRAMEPLAYER_H
#define QGIFER_FRAMEPLAYER_H


#include <QtCore/qstring.h>
#include <QtGui/qimage.h>

namespace core {

    struct Size {
        u_int width;
        u_int height;
    };

    class AbstractFramePlayer {
    public:

        explicit AbstractFramePlayer(const QString &filepath) : filepath(filepath) {
            setSource(filepath);
        }

        virtual ~AbstractFramePlayer() {

        }

        virtual bool setSource(const QString &src) = 0;

        virtual void setPos(u_long pos) = 0;

        virtual double getFPS() {
            return 24; // TODO
        }

        virtual void setFPS() = 0;

        const QString &getSourcePath() const {
            return filepath;
        }

        long getTotalFrames() const {
            return totalFrames;
        }

        const u_long &getPos() const {
            return position;
        }

        virtual QImage getFrame() {
            return frame;
        }

        virtual QImage nextFrame() = 0;

        virtual QImage prevFrame() = 0;

        QImage *getFrame(unsigned long frame_idx) {
            setPos(frame_idx);
            getFrame();
        }


        int estimateInterval() {
            return round(1000.0 / getFPS());
        }


        virtual bool isOpened() {
            return FALSE;
        }

        virtual bool hasNextFrame() {
            return position < totalFrames;
        }

        virtual bool hasPrevFrame() {
            return position > 0;
        }

        virtual void close() = 0;

        virtual QString getCodecName() {
            return codecName;
        }

        virtual core::Size getOriginalSize() {
            return frameSize;
        };

    protected:
        QString filepath;
        core::Size frameSize;
        QImage frame;

        u_long position;
        u_long totalFrames;

        u_int interval;

        QString codecName;
    };

}

#endif //QGIFER_FRAMEPLAYER_H
