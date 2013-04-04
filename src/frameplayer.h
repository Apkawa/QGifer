#ifndef FRAMEPLAYER_H
#define FRAMEPLAYER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QWidget>
#include <QImage>
#include <QStatusBar>
#include <QPixmap>
#include <QMessageBox>
#include "previewwidget.h"
#include "ui_frameplayer.h"

#define FRAME_LIMIT 1800

using namespace cv;

class FramePlayer : public QWidget, public Ui::FramePlayerForm
{
     Q_OBJECT;

public:
     enum Status{Stopped,Playing};
     
     FramePlayer(QWidget* parent);
     virtual ~FramePlayer();

     bool openSource(const QString& src);

     long countFrames() const {return frames;}
     const long& getCurrentPos() const {return currentPos;}
     QImage* getCurrentFrame() {return &currentFrame;}
     int estimateInterval()
     {return vcap.isOpened() ? round(1000.0/vcap.get(CV_CAP_PROP_FPS)) : 0;}
     void setStatusBar(QStatusBar* sb);

     /**
      * Ustawia wskaźnik odczytu na podanej klatce.
      * Metoda ta używa właściwości CV_CAP_PROP_POS_FRAMES, która nie
      * działa właściwie przy większości kodeków, wspierane kodeki
      * wg nazw fourcc to:
      * I420 (raw video)
      * MJPG (motion jpeg)
      * YUV4
      * 
      * @param pos Numer nowej bieżącej klatki
      */
     void setPos(long pos);

#if defined(Q_WS_X11)
     /**
      * Ustawia wskaźnk odczytu na podanej klatce.
      * Metoda jest używana dla kodeków analizujących poprzednie
      * klatki, przez co wymga cofnięcia wskaźnika. Ze względu
      * na czas potrzebny na ponowne wczytanie klatek, FRAME_LIMIT
      * wyznacza maksymalną liczbę klatek filmu dla tych kodeków.
      * 
      * @param pos Numer nowej bieżącej klatki
      */
     void setPosMPEG(long pos);
#endif

     Status getStatus() const {return status;}
     const QString& source() const {return filepath;}
     void setDefaultImage(const QImage& img) 
     {defaultImg = img.copy();}
     void enableAntialiasing(bool enable) 
     {pw->enableAntialiasing(enable);pw->setImage(currentFrame,frame->size());}
     QString codecName();
     void showDefaultScreen()
     {currentFrame = defaultImg;pw->setImage(defaultImg.scaled(frame->size()));}

private:
     VideoCapture vcap;
     QString filepath;
     Status status;
     Size originalSize;
     QImage currentFrame;
     QImage defaultImg;
     PreviewWidget* pw;
     long frames;
     long currentPos;
     int interval;
     int timerId;
     QStatusBar* statusbar;
     bool raw;
     void timerEvent(QTimerEvent*);
     void updateSlider(int pos);
     void updateStatus(Status s);
     void resizeEvent(QResizeEvent*); 
public slots:
     void nextFrame();
     void prevFrame() {seek(currentPos-1);}
     void play();
     void stop();
     void pause();
     void seek(int pos);
     void close();

signals:
     void frameChanged(long);
     void statusUpdated(FramePlayer::Status status);
};

#endif
