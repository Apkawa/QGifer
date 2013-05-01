/************************************************************************
** QGifer
** Copyright (C) 2013 Lukasz Chodyla <chodak166@op.pl>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
************************************************************************/

#ifndef FRAMEPLAYER_H
#define FRAMEPLAYER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QWidget>
#include <QImage>
#include <QStatusBar>
#include <QPixmap>
#include <QMessageBox>
#include "workspace.h"
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
     double fps()
     {return vcap.isOpened() ? vcap.get(CV_CAP_PROP_FPS) : 0;}
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
     {workspace->enableAntialiasing(enable);workspace->setImage(currentFrame,frame->size());}
     QString codecName();
     void showDefaultScreen()
     {currentFrame = defaultImg;workspace->setImage(defaultImg.scaled(frame->size()));}
     Workspace* getWorkspace() {return workspace;}
     void setMedianBlur(int m){medianblur = m;}
     
private:
     VideoCapture vcap;
     int medianblur;
     QString filepath;
     Status status;
     Size originalSize;
     QImage currentFrame;
     QImage defaultImg;
     Workspace* workspace;
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
     void setInterval(int i){interval=i;if(status==Playing){pause();play();}}
     void setFps(double f){ setInterval(1000/f);}
signals:
     void frameChanged(long);
     void statusUpdated(FramePlayer::Status status);
};

#endif
