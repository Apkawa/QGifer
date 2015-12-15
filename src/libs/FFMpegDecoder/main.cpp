// ffmpegDecoder.cpp : Defines the entry point for the console application.
//

#include "ffmpegDecode.h"


/*bool BMPSave(const char *pFileName, AVFrame *frame, int w, int h) {
    bool bResult = false;

    if (frame) {
        FILE *file = fopen(pFileName, "wb");

        if (file) {
            // RGB image
            int imageSizeInBytes = 3 * w * h;
            int headersSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
            int fileSize = headersSize + imageSizeInBytes;

            uint8_t *pData = new uint8_t[headersSize];

            if (pData != NULL) {
                BITMAPFILEHEADER &bfHeader = *((BITMAPFILEHEADER * )(pData));

                bfHeader.bfType = 'MB';
                bfHeader.bfSize = fileSize;
                bfHeader.bfOffBits = headersSize;
                bfHeader.bfReserved1 = bfHeader.bfReserved2 = 0;

                BITMAPINFOHEADER &bmiHeader = *((BITMAPINFOHEADER * )(pData + headersSize - sizeof(BITMAPINFOHEADER)));

                bmiHeader.biBitCount = 3 * 8;
                bmiHeader.biWidth = w;
                bmiHeader.biHeight = h;
                bmiHeader.biPlanes = 1;
                bmiHeader.biSize = sizeof(bmiHeader);
                bmiHeader.biCompression = BI_RGB;
                bmiHeader.biClrImportant = bmiHeader.biClrUsed =
                bmiHeader.biSizeImage = bmiHeader.biXPelsPerMeter =
                bmiHeader.biYPelsPerMeter = 0;

                fwrite(pData, headersSize, 1, file);

                uint8_t *pBits = frame->data[0] + frame->linesize[0] * h - frame->linesize[0];
                int nSpan = -frame->linesize[0];

                int numberOfBytesToWrite = 3 * w;

                for (size_t i = 0; i < h; ++i, pBits += nSpan) {
                    fwrite(pBits, numberOfBytesToWrite, 1, file);
                }

                bResult = true;
                delete[] pData;
            }

            fclose(file);
        }
    }

    return bResult;
}*/


int main() {
    FFmpegDecoder decoder;
    char * filename = "/home/apkawa/tmp/SampleVideo_360x240_1mb.mp4";
    if (decoder.OpenFile(filename)) {
        int w = decoder.GetWidth();
        int h = decoder.GetHeight();
        printf("video size %ix%i\n", w, h);
        printf("total frames %i\n", decoder.getTotalFrames());
        decoder.SeekFrame(1);
        for (int i = 0; i < 5000; i++) {
            AVFrame *frame = decoder.GetNextFrame();
            if (frame && frame->key_frame == 1) {
                printf("frame i=%i; %i is key_frame dts=%i \n", i, frame->coded_picture_number, frame->pkt_dts);
            }
        }
        decoder.FindKeyFrame(50);

        decoder.CloseFile();
    } else {
        printf("Can't open file %s\n", filename);
    }

    return 0;
}

