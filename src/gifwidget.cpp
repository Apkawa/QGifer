#include "gifwidget.h"
#include <QProgressDialog>

GifWidget::GifWidget(
     QWidget* parent, 
     Qt::WindowFlags f): QWidget(parent,f), timerId(-1), currentFrame(-1)
{
     setupUi(this);
     createActions();
}

GifWidget::~GifWidget()
{

}

void GifWidget::addFrame(const QImage& f)
{
     QImage i(f);
     i = i.mirrored().convertToFormat(QImage::Format_RGB888);
     gif.resize(i.width(),i.height());
     gif.prepareFrame(&i, palette);
     prevFrames.append(i.mirrored());
     currentFrame = 0;
}

void GifWidget::createActions()
{
     QAction* a = new QAction(tr("Save gif..."), this);
     a->setIcon(QIcon(":/res/floppy.png"));
     saveButton->setDefaultAction(a);
     connect(a, SIGNAL(triggered()), this, SLOT(save()));

     a = new QAction(tr("Play"), this);
     a->setIcon(QIcon(":/res/start.png"));
     playButton->setDefaultAction(a);
     connect(a, SIGNAL(triggered()), this, SLOT(play()));
     
     a = new QAction(tr("Pause"), this);
     a->setIcon(QIcon(":/res/pause.png"));
     pauseButton->setDefaultAction(a);
     connect(a, SIGNAL(triggered()), this, SLOT(pause()));

}

void GifWidget::play()
{
     if(timerId == -1)
     {
	  timerId = startTimer(intervalBox->value());
	  currentFrame = 0;
     }
     else
     {
	  pause();
	  play();
     }
     intervalBox->setEnabled(false);
}

void GifWidget::pause()
{
     killTimer(timerId);
     timerId = -1;
     intervalBox->setEnabled(true);
}

void GifWidget::save()
{
     qDebug() << "saving gif...";
     if(!prevFrames.size())
	  return;
     pause();
     gif.setDuration((float)intervalBox->value()/1000);
     // QProgressDialog pd(tr("Rendering frames..."), tr("Abort"), 0, prevFrames.size() , this);
     // pd.setWindowModality(Qt::WindowModal);
     // pd.show();
     // qApp->processEvents();
     //  for(int i=0;i<prevFrames.size();i++)
     // {
     // 	  pd.setValue(i);
     // 	  gc.addFrame(prevFrames.at(i));
     // }
     // pd.setValue(prevFrames.size());
     gif.save("/home/chodak/testgif.gif");
}

void GifWidget::timerEvent(QTimerEvent*)
{
     preview->setImage(prevFrames.at(currentFrame));
     currentFrame++;
     if(currentFrame >= prevFrames.size())
	  currentFrame = 0;
}
