#include "mainwindow.h"
#include <QTimer>

MainWindow::MainWindow()
{
     setupUi(this);
     player->controlPanel->hide();
     player->setStatusBar(statusbar);
     
     upStartButton->setIcon(QIcon(":/res/fromimg.png"));
     upStartButton->setDefaultAction(actionSetAsStart);
     upStopButton->setDefaultAction(actionSetAsStop);
     upStopButton->setIcon(QIcon(":/res/fromimg.png"));

     connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));
     connect(actionExtractGif, SIGNAL(triggered()), this, SLOT(extractGif()));
     connect(updatePalButton, SIGNAL(clicked()), this, SLOT(updatePalette()));

     connect(actionSetAsStart, SIGNAL(triggered()), this, SLOT(startFromCurrent()));
     connect(actionSetAsStop, SIGNAL(triggered()), this, SLOT(stopFromCurrent()));

     connect(actionOpenVideo, SIGNAL(triggered()), this, SLOT(openVideo()));
     connect(actionCloseVideo, SIGNAL(triggered()), player, SLOT(close()));
     connect(actionPlay, SIGNAL(triggered()), player, SLOT(play()));
     connect(actionStop, SIGNAL(triggered()), player, SLOT(stop()));
     connect(actionPause, SIGNAL(triggered()), player, SLOT(pause()));
     connect(actionNextFrame, SIGNAL(triggered()), player, SLOT(nextFrame()));
     connect(actionPrevFrame, SIGNAL(triggered()), player, SLOT(prevFrame()));

     //test
     //openVideo("/home/chodak/rec/tkw540.avi");
}

MainWindow::~MainWindow()
{

}

void MainWindow::openVideo()
{
     QString path = QFileDialog::getOpenFileName(
	  this, tr("Open video file"), qApp->applicationDirPath(),
	  "Video files (*.avi *.mp4 *.mpg *.ogv *.flv);;All files (*.*)");
     if(!path.isEmpty())
	  if(!openVideo(path))
	       QMessageBox::critical(this, tr("Error"),tr("The player failed to load the video file!"));
}

bool MainWindow::openVideo(const QString& path)
{
     if(player->openSource(path))
     {
         player->nextFrame();
	 startBox->setMaximum(player->countFrames());
	 stopBox->setMaximum(player->countFrames());
	 startBox->setValue(0);
	 stopBox->setValue(0);
	 updatePalette();
	 return true;
     }
     startBox->setMaximum(0);
     stopBox->setMaximum(0);

     player->showDefaultScreen();
     return false;
}


void MainWindow::extractGif()
{
     if(startBox->value() > stopBox->value())
     {
	  QMessageBox::critical(this, tr("Error"),tr("The range is invalid!"));
	  return;
     }

     if(!paletteWidget->map())
	  updatePalette();

     if(!paletteWidget->map())
     {
	  QMessageBox::critical(this, tr("Error"),tr("Invalid color map!"));
	  return;
     }

     GifWidget* g = new GifWidget();
     g->setAttribute(Qt::WA_DeleteOnClose, true);
     g->setPalette(paletteWidget->map());
     for(long i=startBox->value();i<=stopBox->value();i++)
     {
	  player->seek(i);
	  g->addFrame(player->getCurrentFrame()->scaled(widthBox->value(),heightBox->value()));
     }
     g->show();
     g->play();
     QTimer::singleShot(100,g,SLOT(adjustWidgetSize())); //TODO find another way....
}

void MainWindow::updatePalette()
{
     paletteWidget->fromImage(*player->getCurrentFrame(), pow(2,paletteBox->value()));
}
