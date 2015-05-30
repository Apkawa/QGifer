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