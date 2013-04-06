
#include "frameplayer.h"

FramePlayer::FramePlayer(QWidget* parent):QWidget(parent),frames(0),originalSize(Size(0,0)),
				    currentPos(-1),timerId(-1), status(Stopped),statusbar(NULL),
					  raw(false),interval(40)
{
     setupUi(this);
     pw = new PreviewWidget(frame);
     defaultImg = QImage(":/res/playerdefault2.png");
     connect(playButton, SIGNAL(clicked()), this, SLOT(play()));
     connect(stopButton, SIGNAL(clicked()), this, SLOT(stop()));
     connect(pauseButton, SIGNAL(clicked()), this, SLOT(pause()));
     connect(nextButton, SIGNAL(clicked()), this, SLOT(nextFrame()));
     connect(prevButton, SIGNAL(clicked()), this, SLOT(prevFrame()));
     connect(ejectButton, SIGNAL(clicked()), this, SLOT(close()));
     connect(slider, SIGNAL(valueChanged(int)), this, SLOT(seek(int)));
}

FramePlayer::~FramePlayer()
{
     //delete pw;
}

bool FramePlayer::openSource(const QString& src)
{
     if(vcap.isOpened())
	  vcap.release();
     vcap.open(src.toStdString());
     originalSize.width = vcap.get(CV_CAP_PROP_FRAME_WIDTH);
     originalSize.height = vcap.get(CV_CAP_PROP_FRAME_HEIGHT);
     killTimer(timerId);
     timerId = -1;
     currentPos = -1;
     if(!vcap.isOpened())
     {
	  qDebug() << "FramePlayer::openSource cannot load video: " << src;
	  close();
	  return false;
     }

     qDebug() << "FramePlayer::openSource new video loaded: " << src;
#if defined(Q_WS_X11)
     QString codec = codecName();
     raw = (codec == "MJPG" || codec == "I420" || codec == "YUV4");
     qDebug() << "codec name: " << codecName();
#endif

     filepath = src;
     frames = vcap.get(CV_CAP_PROP_FRAME_COUNT);
     qDebug() << "total frames: " << frames;

#if defined(Q_WS_X11)
     if(frames > FRAME_LIMIT && !raw)
     {
	  QMessageBox::critical(this, tr("Error"),tr("This file is coded with \"")+codec+tr("\" codec and contains more than ")+QString::number(FRAME_LIMIT)+tr(" frames. This codec is not supported for full-length videos yet, please cut your video or use other codec like motion JPEG or YUV4."));
	  close();
	  return false;
     }
#endif
     interval = estimateInterval();
     if(!interval) 
	  interval = 40;
     //fpsLabel->setText("("+QString::number(1000/interval)+" FPS)");
     slider->setMaximum(countFrames()-1);
     //updateSlider(0);
     slider->setValue(0);
     return true;
}

void FramePlayer::close()
{
     stop();
     vcap.release();
     //fakeFrames += frames;
     frames = 0;
     filepath.clear();
     originalSize = Size(0,0);
     slider->setValue(0);
     showDefaultScreen();
     updateStatus(status);
}

void FramePlayer::nextFrame()
{
     //qDebug() << "FramePlayer::nextFrame(): frames: " << frames << ", fakeFrames: " << fakeFrames << ", currentPos: " << currentPos;
     if(!vcap.isOpened())
	  return;

     if(vcap.isOpened() && currentPos+1 <= frames-1)
     {
	  //qDebug() << "next REAL frame...";
	  Mat m;
	  //vcap.grab();
	  //vcap.retrieve(m);
	  vcap >> m;
	  //currentPos = vcap.get(CV_CAP_PROP_POS_FRAMES); //videocapture zlicza roznie przy roznych kodekach
	  currentPos++;
	  if(currentPos>=frames)
	       currentPos = frames-1;
	  //qDebug() << "current pos: " << currentPos << "/" << frames;
	  //cvtColor(m,m,CV_BGR2RGB);
	  currentFrame = QImage((uchar*)m.data, m.cols, m.rows, m.step, 
				QImage::Format_RGB888).rgbSwapped();
     }
     else 
	  return;

     emit frameChanged(currentPos);
     pw->setImage(currentFrame,frame->size());
     updateSlider(currentPos);
     
     updateStatus(timerId == -1 ? Stopped : Playing);
}

void FramePlayer::play()
{
     if(!countFrames())
	  return;

     if(currentPos == countFrames()-1)
	  seek(0);

     if(timerId==-1)
	  timerId = startTimer(interval);
     updateStatus(Playing);
}

void FramePlayer::stop()
{
     pause();
     seek(0);
     //nextFrame(); //tymczasowo podwójnie aby zaktualizować podgląd
     //seek(0);
}

void FramePlayer::pause()
{
     killTimer(timerId);
     timerId = -1;
     updateStatus(Stopped);
}

void FramePlayer::setPos(long pos)
{
     if(!vcap.isOpened())
	  return;

     //------ tymczasowy sposob ze wzgledu na bug gubienia klatek w opencv
      // vcap.release();
      // vcap.open(filepath.toStdString());
      //--------------------------------------

     if(pos < 0) pos = 0;
     if(pos >= frames) pos = frames-1;
     currentPos = pos;
     vcap.set(CV_CAP_PROP_POS_FRAMES, currentPos);
     currentPos--;
     nextFrame();
}

#if defined(Q_WS_X11)
void FramePlayer::setPosMPEG(long pos)
{
      if(!vcap.isOpened())
	  return;

      //------ tymczasowy sposob ze wzgledu na bug gubienia klatek w opencv
      vcap.release();
      vcap.open(filepath.toStdString());
      //--------------------------------------
      
     if(pos < 0) pos = 0;
     if(pos >= frames) pos = frames-1;
     currentPos = pos;
     //vcap.set(CV_CAP_PROP_POS_FRAMES, 0);
     while(pos-- > 0)
      	  vcap.grab();
     currentPos--; //aktualizujemy klatkę ale nie pozycję
     nextFrame();
}
#endif

void FramePlayer::seek(int pos)
{
     //qDebug() << "seeking pos: " << pos;
     if(pos < frames)
     {
#if defined(Q_WS_X11)
	  if(!raw) 
	       setPosMPEG(pos);
	  else 
#endif
	       setPos(pos);
     }
     else if(pos >= frames && pos < countFrames())
     {
	  currentPos = pos-1;
	  nextFrame();
     }
}

void FramePlayer::timerEvent(QTimerEvent*)
{
     //qDebug() << "tick";
     nextFrame();
     if(currentPos >= countFrames()-1)
	  stop();
}


void FramePlayer::updateSlider(int pos)
{
     if(pos < 0) pos = 0;
     disconnect(slider, SIGNAL(valueChanged(int)), this, SLOT(seek(int)));
     slider->setValue(pos);
     connect(slider, SIGNAL(valueChanged(int)), this, SLOT(seek(int)));
}

void FramePlayer::updateStatus(Status s)
{
     QString info = "";
     status = s;
     info = (status == Stopped ? "Stopped" : "Playing");

     playButton->setEnabled(status == Stopped);
     prevButton->setEnabled(status == Stopped);
     nextButton->setEnabled(status == Stopped);
     slider->setEnabled(status == Stopped);

     long total = countFrames();
     if(total)
	  info += ", frame: " + QString::number(currentPos)+"/"+QString::number(total-1) +
	       " ("+QString::number(total)+" in total)";
     else
	  info += ", no frames";
     slider->setMaximum(total == 0 ? 0 : total-1);
     if(statusbar)
	  statusbar->showMessage(info);
     else
	  statusLabel->setText(info);
     emit statusUpdated(s);
}


void FramePlayer::resizeEvent(QResizeEvent*)
{
     //qDebug() << "player resize event";
     pw->setImage(currentFrame,frame->size());
     pw->setFixedSize(size());
}

void FramePlayer::setStatusBar(QStatusBar* sb)
{
     statusbar = sb;
     statusLabel->setVisible(sb==NULL);
}

QString FramePlayer::codecName()
{
     if(!vcap.isOpened())
	  return "";
     int ex = static_cast<int>(vcap.get(CV_CAP_PROP_FOURCC));
     char EXT[] = {(char)(ex & 0XFF) , (char)((ex & 0XFF00) >> 8),(char)((ex & 0XFF0000) >> 16),(char)((ex & 0XFF000000) >> 24), 0};
     return QString(EXT);
}
