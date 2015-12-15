/*
	It is FFmpeg decoder class. Sample for article from unick-soft.ru
    https://github.com/UnickSoft/FFMpeg-decode-example
*/

#ifndef __FFMPEG_DECODER__
#define __FFMPEG_DECODER__

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}

#define min(a, b) (a > b ? b : a)

#define MAX_AUDIO_PACKET (2 * 1024 * 1024)

#include <string>

class FFmpegDecoder {
    // constructor.
public:
    FFmpegDecoder() : pImgConvertCtx(NULL), videoBaseTime(0.0),
                      videoFramePerSecond(0.0), isOpen(false), audioStreamIndex(-1), videoStreamIndex(-1),
                      pAudioCodec(NULL), pAudioCodecCtx(NULL), pVideoCodec(NULL), pVideoCodecCtx(NULL),
                      pFormatCtx(NULL) { ; }

    // destructor.
    virtual ~FFmpegDecoder() {
        CloseFile();
    }

    // Open file
    virtual bool OpenFile(const char *inputFile);

    // Close file and free resourses.
    virtual bool CloseFile();

    // Return next frame FFmpeg.
    virtual AVFrame *GetNextFrame();

    virtual void SeekFrame(int64_t frame_idx);

    int GetWidth() {
        return width;
    }

    int GetHeight() {
        return height;
    }
    void FindKeyFrame(int64_t frame);

    u_long getTotalFrames() {
        return pVideoCodecCtx->refcounted_frames;
    }

    void printDuration() {
        if (pFormatCtx->duration != AV_NOPTS_VALUE) {
            int hours, mins, secs, us;
            int64_t duration = pFormatCtx->duration + 5000;
            int total_secs = secs = duration / AV_TIME_BASE;
            us    = duration % AV_TIME_BASE;
            mins  = total_secs / 60;
            secs %= 60;
            hours = mins / 60;
            mins %= 60;
            double f_total_secs = (double)total_secs + (((100.0 * us) / AV_TIME_BASE) / 100.0) ;
            av_log(NULL, AV_LOG_INFO, "%02d:%02d:%02d.%02d\n", hours, mins, secs, (100 * us) / AV_TIME_BASE);
            av_log(NULL, AV_LOG_INFO, "seconds: %f\n", f_total_secs);
            av_log(NULL, AV_LOG_INFO, "frames: %f\n", videoFramePerSecond * f_total_secs);
        }

    }


private:
    // open video stream.
    bool OpenVideo();

    // close video stream.
    void CloseVideo();


    // return rgb image
    AVFrame *GetRGBAFrame(AVFrame *pFrameYuv);


    // Decode video buffer.
    bool DecodeVideo(const AVPacket *avpkt, AVFrame *pOutFrame);

    // FFmpeg file format.
    AVFormatContext *pFormatCtx;

    // FFmpeg codec context.
    AVCodecContext *pVideoCodecCtx;

    // FFmpeg codec for video.
    AVCodec *pVideoCodec;

    // FFmpeg codec context for audio.
    AVCodecContext *pAudioCodecCtx;

    // FFmpeg codec for audio.
    AVCodec *pAudioCodec;

    // Video stream number in file.
    int videoStreamIndex;

    // Audio stream number in file.
    int audioStreamIndex;

    // File is open or not.
    bool isOpen;

    // Video frame per seconds.
    double videoFramePerSecond;

    // FFmpeg timebase for video.
    int64_t videoBaseTime;


    // FFmpeg context convert image.
    struct SwsContext *pImgConvertCtx;

    // Width of image
    int width;

    // Height of image
    int height;

};

#endif