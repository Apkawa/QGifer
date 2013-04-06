#include "mainwindow.h"
#include <QTimer>

MainWindow::MainWindow()
{
     setupUi(this);
     player->controlPanel->hide();
     player->setStatusBar(statusbar);
     
     set = new QSettings("QGifer","QGifer");
     
     upStartButton->setIcon(QIcon(":/res/fromimg.png"));
     upStartButton->setDefaultAction(actionSetAsStart);
     upStopButton->setDefaultAction(actionSetAsStop);
     upStopButton->setIcon(QIcon(":/res/fromimg.png"));

     multiSlider->setSkin(QPixmap(":/res/multislider/bar.png"),
			  QPixmap(":/res/multislider/midbar.png"),
			  QPixmap(":/res/multislider/left.png"),
			  QPixmap(":/res/multislider/right.png"),
			  QPixmap(":/res/multislider/a.png").
			  scaled(16,16,Qt::IgnoreAspectRatio,Qt::SmoothTransformation),
			  QPixmap(":/res/multislider/b.png").
			  scaled(16,16,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
     multiSlider->setMaximum(0);
     multiSlider->setPosA(0);
     multiSlider->setPosB(0);

     connect(actionExit, SIGNAL(triggered()), this, SLOT(close()));
     connect(actionOptimizer, SIGNAL(triggered()), this, SLOT(runOptimizer()));
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

     connect(stopBox, SIGNAL(valueChanged(int)), this, SLOT(stopChanged(int)));
     connect(startBox, SIGNAL(valueChanged(int)), this, SLOT(startChanged(int)));

     connect(multiSlider, SIGNAL(posAChanged(int)), this, SLOT(posAChanged(int)));
     connect(multiSlider, SIGNAL(posBChanged(int)), this, SLOT(posBChanged(int)));

     connect(fpsBox, SIGNAL(valueChanged(double)), player, SLOT(setFps(double)));
     connect(player, SIGNAL(frameChanged(long)), this, SLOT(frameChanged(long)));

     //test
     //openVideo("/home/chodak/rec/tkw540.avi");
}

MainWindow::~MainWindow()
{
     delete set;
}

void MainWindow::openVideo()
{
     QString path = QFileDialog::getOpenFileName(
	  this, tr("Open video file"), 
	  set->value("last_video_dir",qApp->applicationDirPath()).toString(),
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
	 multiSlider->setMaximum(player->countFrames());
	 startBox->setValue(0);
	 stopBox->setValue(1);
	 fpsBox->setValue(player->fps());
	 set->setValue("last_video_dir",QFileInfo(path).absoluteDir().absolutePath());
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
     {
	  player->seek(startBox->value());
	  updatePalette();
     }

     if(!paletteWidget->map())
     {
	  QMessageBox::critical(this, tr("Error"),tr("Invalid color map!"));
	  return;
     }

     GifWidget* g = new GifWidget(set);
     connect(g, SIGNAL(gifSaved(const QString&)), this, SLOT(gifSaved(const QString&)));
     g->setAttribute(Qt::WA_DeleteOnClose, true);
     g->setPalette(paletteWidget->map());
     for(long i=startBox->value();i<=stopBox->value();i++)
     {
	  player->seek(i);
	  g->addFrame(player->getCurrentFrame()->scaled(widthBox->value(),heightBox->value(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
     }
     g->show();
     g->play();
     player->pause();
     QTimer::singleShot(100,g,SLOT(adjustWidgetSize())); //TODO find another way....
}

void MainWindow::updatePalette()
{
     paletteWidget->fromImage(*player->getCurrentFrame(), pow(2,paletteBox->value()));
}

void MainWindow::posAChanged(int v)
{
     disconnect(startBox, SIGNAL(valueChanged(int)), this, SLOT(startChanged(int)));
     startBox->setValue(v);
     connect(startBox, SIGNAL(valueChanged(int)), this, SLOT(startChanged(int)));
}

void MainWindow::posBChanged(int v)
{
     disconnect(stopBox, SIGNAL(valueChanged(int)), this, SLOT(stopChanged(int)));
     stopBox->setValue(v);
     connect(stopBox, SIGNAL(valueChanged(int)), this, SLOT(stopChanged(int)));
}

void MainWindow::frameChanged(long f)
{
     if(laRadio->isChecked() && f >= player->countFrames()-1)
	  player->seek(0);
     else if(ssRadio->isChecked() && (f >= stopBox->value() || f < startBox->value()) && 
	     stopBox->value() > startBox->value())
	  player->seek(startBox->value());
     else if(ssRadio->isChecked() && f >= stopBox->value() && 
	     stopBox->value() < startBox->value())
     {
	  QMessageBox::warning(this,tr("Warning"),tr("The range seems to be invalid!"));
	  player->pause();
     }
}

void MainWindow::gifSaved(const QString& path)
{
     set->setValue("last_gif", path);
     set->setValue("last_gif_dir", QFileInfo(path).absoluteDir().absolutePath());
     if(set->value("show_optimizer",false).toBool())
	  runOptimizer();
}
