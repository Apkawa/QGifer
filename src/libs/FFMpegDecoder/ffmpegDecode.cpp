/*
	It is FFmpeg decoder class. Sample for article from unick-soft.ru
*/

#include "ffmpegDecode.h"


bool FFmpegDecoder::OpenFile(const char * inputFile) {
    CloseFile();

    // Register all components
    av_register_all();

    // Open media file.
    if (avformat_open_input(&pFormatCtx, inputFile, NULL, NULL) != 0) {
        CloseFile();
        return false;
    }

    // Get format info.
    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        CloseFile();
        return false;
    }

    // open video and audio stream.
    bool hasVideo = OpenVideo();

    if (!hasVideo) {
        CloseFile();
        return false;
    }

    isOpen = true;

    // Get file information.
    if (videoStreamIndex != -1) {
        videoFramePerSecond = av_q2d(pFormatCtx->streams[videoStreamIndex]->r_frame_rate);
        // Need for convert time to ffmpeg time.
//        videoBaseTime = (int64_t(pVideoCodecCtx->time_base.num) * AV_TIME_BASE) / int64_t(pVideoCodecCtx->time_base.den);
        videoBaseTime = av_q2d(pFormatCtx->streams[videoStreamIndex]->time_base);
        double testBaseTime = av_q2d(pFormatCtx->streams[videoStreamIndex]->time_base);

        printf("FPS %f\n", videoFramePerSecond);
        printDuration();
        av_dump_format(pFormatCtx, videoStreamIndex, inputFile, 0);

    }
    return true;
}




bool FFmpegDecoder::CloseFile() {
    isOpen = false;

    // Close video and audio.
    CloseVideo();

    if (pFormatCtx) {
        pFormatCtx = NULL;
    }

    return true;
}


AVFrame *FFmpegDecoder::GetNextFrame() {
    AVFrame *res = NULL;

    if (videoStreamIndex != -1) {
        AVFrame *pVideoYuv = avcodec_alloc_frame();
        AVPacket packet;

        if (isOpen) {
            // Read packet.
            while (av_read_frame(pFormatCtx, &packet) >= 0) {
                int64_t pts = 0;
                pts = (packet.dts != AV_NOPTS_VALUE) ? packet.dts : 0;

                if (packet.stream_index == videoStreamIndex) {
                    // Decode frame
                    bool isDecodeComplite = DecodeVideo(&packet, pVideoYuv);
                    if (isDecodeComplite) {
                        res = pVideoYuv;
                    }
                    break;
                }
                av_free_packet(&packet);
                packet = AVPacket();
            }

            av_free(pVideoYuv);
        }
    }
    return res;
}


void FFmpegDecoder::SeekFrame(long frame_idx) {
    int64_t frame_ts = frame_idx * videoBaseTime;

    printf("seek to %i, %i, %f", frame_idx, frame_ts, videoBaseTime);
    if (av_seek_frame(pFormatCtx, videoStreamIndex, frame_idx, 0) < 0) {
        printf("fail seek %i", frame_ts);
    }
}

void FFmpegDecoder::FindKeyFrame(int64_t frame) {
    int preceedingKeyframe = av_index_search_timestamp(pFormatCtx->streams[videoStreamIndex], frame, AVSEEK_FLAG_BACKWARD);
    printf("%i", preceedingKeyframe);
//    int64_t nearestKeyframePts = preceedingKeyframe * pFormatCtx->streams[videoStreamIndex]->time_base.den;


}


AVFrame *FFmpegDecoder::GetRGBAFrame(AVFrame * pFrameYuv) {
    AVFrame *frame = NULL;
    int width = pVideoCodecCtx->width;
    int height = pVideoCodecCtx->height;
    int bufferImgSize = avpicture_get_size(PIX_FMT_BGR24, width, height);
    frame = avcodec_alloc_frame();
    uint8_t *buffer = (uint8_t *) av_mallocz(bufferImgSize);
    if (frame) {
        avpicture_fill((AVPicture *) frame, buffer, PIX_FMT_BGR24, width, height);
        frame->width = width;
        frame->height = height;
        //frame->data[0] = buffer;

        sws_scale(pImgConvertCtx, pFrameYuv->data, pFrameYuv->linesize,
                  0, height, frame->data, frame->linesize);
    }

    return (AVFrame *) frame;
}

bool FFmpegDecoder::OpenVideo() {
    bool res = false;

    if (pFormatCtx) {
        videoStreamIndex = -1;

        for (unsigned int i = 0; i < pFormatCtx->nb_streams; i++) {
            if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
                videoStreamIndex = i;
                pVideoCodecCtx = pFormatCtx->streams[i]->codec;
                pVideoCodec = avcodec_find_decoder(pVideoCodecCtx->codec_id);

                if (pVideoCodec) {
                    res = !(avcodec_open2(pVideoCodecCtx, pVideoCodec, NULL) < 0);
                    width = pVideoCodecCtx->coded_width;
                    height = pVideoCodecCtx->coded_height;
                }

                break;
            }
        }

        if (!res) {
            CloseVideo();
        }
        else {
            pImgConvertCtx = sws_getContext(pVideoCodecCtx->width, pVideoCodecCtx->height,
                                            pVideoCodecCtx->pix_fmt,
                                            pVideoCodecCtx->width, pVideoCodecCtx->height,
                                            PIX_FMT_BGR24,
                                            SWS_BICUBIC, NULL, NULL, NULL);
        }
    }

    return res;
}

bool FFmpegDecoder::DecodeVideo(const AVPacket *avpkt, AVFrame *pOutFrame) {
    bool res = false;

    if (pVideoCodecCtx) {
        if (avpkt && pOutFrame) {
            int got_picture_ptr = 0;
            int videoFrameBytes = avcodec_decode_video2(pVideoCodecCtx, pOutFrame, &got_picture_ptr, avpkt);
            res = (videoFrameBytes > 0);
        }
    }
    return res;
}


void FFmpegDecoder::CloseVideo() {
    if (pVideoCodecCtx) {
        avcodec_close(pVideoCodecCtx);
        pVideoCodecCtx = NULL;
        pVideoCodec = NULL;
        videoStreamIndex = 0;
    }
}

