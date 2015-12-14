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
        }

        virtual ~AbstractFramePlayer() {

        }

        virtual bool setSource(const QString &src) = 0;

        virtual void setPos(u_long pos) = 0;

        virtual double getFPS() {
            return 24; // TODO
        }


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
        virtual QImage *getPointerFrame() {
            return &frame;
        }

        virtual void seek(u_long pos) = 0;

        virtual QImage nextFrame() = 0;

        virtual QImage prevFrame() = 0;

        QImage getFrame(unsigned long frame_idx) {
            setPos(frame_idx);
            return getFrame();
        }


        int estimateInterval() {
            int calculated_interval = round(1000.0 / getFPS());
            if (calculated_interval) {
                return calculated_interval;
            }
            return 40;
        }


        virtual bool isOpened() {
            return FALSE;
        }

        virtual bool hasNextFrame() {
            return isOpened() && position + 1 < totalFrames;
        }

        virtual bool hasPrevFrame() {
            return isOpened() && position - 1 > 0;
        }

        virtual void close() = 0;

        virtual QString getCodecName() {
            return codecName;
        }

        virtual core::Size getOriginalSize() const{
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
