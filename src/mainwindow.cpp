#include "mainwindow.h"
#include <QTimer>
#include <QProgressDialog>

MainWindow::MainWindow()
{
     setupUi(this);
     player->controlPanel->hide();
     player->setStatusBar(statusbar);
     //player->showDefaultScreen();
     set = new QSettings("QGifer","QGifer");
     
     // upStartButton->setIcon(QIcon(":/res/fromimg.png"));
     // upStartButton->setDefaultAction(actionSetAsStart);
     // upStopButton->setDefaultAction(actionSetAsStop);
     // upStopButton->setIcon(QIcon(":/res/fromimg.png"));

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
     connect(actionUpdatePalette, SIGNAL(triggered()), this, SLOT(updatePalette()));

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

     connect(zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(zoomChanged(int)));
     connect(ratioBox, SIGNAL(stateChanged(int)), this, SLOT(ratioChanged(int)));
     connect(smoothBox, SIGNAL(stateChanged(int)), this, SLOT(smoothChanged(int)));
     connect(marginBox, SIGNAL(stateChanged(int)), this, SLOT(marginBoxChanged(int)));

     connect(redSlider, SIGNAL(valueChanged(int)), this, SLOT(balanceChanged()));
     connect(greenSlider, SIGNAL(valueChanged(int)), this, SLOT(balanceChanged()));
     connect(blueSlider, SIGNAL(valueChanged(int)), this, SLOT(balanceChanged()));
     connect(balanceBox, SIGNAL(stateChanged(int)), this, SLOT(balanceChanged()));
     connect(resetBalanceButton, SIGNAL(clicked()), this, SLOT(resetBalance()));

     //marginesy
     connectMargins();
     connect(player->previewWidget(), SIGNAL(marginsChanged()), this, SLOT(marginsChanged()));
     //test
     //openVideo("/home/chodak/rec/tkw540.avi");

     lock(true);
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
	 lock(false);
	 return true;
     }
     startBox->setMaximum(0);
     stopBox->setMaximum(0);

     player->showDefaultScreen();
     lock(true);
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
     player->pause();
     GifWidget* g = new GifWidget(set);
     connect(g, SIGNAL(gifSaved(const QString&)), this, SLOT(gifSaved(const QString&)));
     g->setAttribute(Qt::WA_DeleteOnClose, true);
     g->setPalette(paletteWidget->map());
     QProgressDialog pd("Rendering frames...", "Abort", startBox->value(), 
			      stopBox->value(), this);
     pd.setWindowModality(Qt::WindowModal);
     qApp->processEvents();
     for(long i=startBox->value();i<=stopBox->value();i++)
     {
	  pd.setValue(i);
	  g->addFrame(finalFrame(i));
     }
     pd.setValue(stopBox->value());
     g->show();
     g->play();
     player->pause();
     QTimer::singleShot(100,g,SLOT(adjustWidgetSize())); //TODO find another way....
}

void MainWindow::updatePalette()
{
     paletteWidget->fromImage(finalFrame(player->getCurrentPos()), pow(2,paletteBox->value()));
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


     
     if(balanceBox->isChecked() && 
	(redSlider->value() || blueSlider->value() || greenSlider->value()))
     {
	  // PreviewWidget::applyBalance(player->previewWidget()->getImage(),
	  // 			 redSlider->value(),
	  // 			 greenSlider->value(),
	  // 			 blueSlider->value());
	  // player->previewWidget()->update();
	  balanceChanged();
     }
}

void MainWindow::gifSaved(const QString& path)
{
     set->setValue("last_gif", path);
     set->setValue("last_gif_dir", QFileInfo(path).absoluteDir().absolutePath());
     if(set->value("show_optimizer",false).toBool())
	  runOptimizer();
}

void MainWindow::zoomChanged(int v)
{
     zoomLabel->setText(tr("Zoom")+" ("+QString::number(v)+"%):");
     player->previewWidget()->setZoom((double)v/100.0);
     if(player->getStatus() != FramePlayer::Playing)
	  player->seek(player->getCurrentPos());
}

void MainWindow::ratioChanged(int s)
{
     player->previewWidget()->keepAspectRatio(s == Qt::Checked);
     if(player->getStatus() != FramePlayer::Playing)
	  player->seek(player->getCurrentPos());
}

void MainWindow::smoothChanged(int s)
{
     player->previewWidget()->enableAntialiasing(s == Qt::Checked);
     if(player->getStatus() != FramePlayer::Playing)
	  player->seek(player->getCurrentPos());
}

void MainWindow::applyMargins()
{
     *player->previewWidget()->margins() = QMargins(
	  leftSpin->value(),topSpin->value(),
	  rightSpin->value(),bottomSpin->value()); 
     player->previewWidget()->update();
}

void MainWindow::marginsChanged()
{
     disconnectMargins();
     QMargins* m = player->previewWidget()->margins();
     leftSpin->setValue(m->left());
     rightSpin->setValue(m->right());
     topSpin->setValue(m->top());
     bottomSpin->setValue(m->bottom());
     connectMargins();
}

void MainWindow::connectMargins()
{
     connect(leftSpin, SIGNAL(valueChanged(int)), this, SLOT(applyMargins()));
     connect(topSpin, SIGNAL(valueChanged(int)), this, SLOT(applyMargins()));
     connect(rightSpin, SIGNAL(valueChanged(int)), this, SLOT(applyMargins()));
     connect(bottomSpin, SIGNAL(valueChanged(int)), this, SLOT(applyMargins()));
}

void MainWindow::disconnectMargins()
{
     disconnect(leftSpin, SIGNAL(valueChanged(int)), this, SLOT(applyMargins()));
     disconnect(topSpin, SIGNAL(valueChanged(int)), this, SLOT(applyMargins()));
     disconnect(rightSpin, SIGNAL(valueChanged(int)), this, SLOT(applyMargins()));
     disconnect(bottomSpin, SIGNAL(valueChanged(int)), this, SLOT(applyMargins()));
}

void MainWindow::marginBoxChanged(int s)
{
     player->previewWidget()->enableMargins(s == Qt::Checked);
     player->previewWidget()->update();
}

void MainWindow::balanceChanged()
{
     if(!balanceBox->isChecked())
	  return;
     player->previewWidget()->setImage(*player->getCurrentFrame(),player->previewWidget()->getImage()->size());
     PreviewWidget::applyBalance(player->previewWidget()->getImage(),
				 redSlider->value(),
				 greenSlider->value(),
				 blueSlider->value());
     player->previewWidget()->update();

     redLabel->setText(tr("Red (")+QString::number(redSlider->value())+"):");
     greenLabel->setText(tr("Green (")+QString::number(greenSlider->value())+"):");
     blueLabel->setText(tr("Blue (")+QString::number(blueSlider->value())+"):");
}

void MainWindow::resetBalance()
{
     redSlider->setValue(0);
     greenSlider->setValue(0);
     blueSlider->setValue(0);
}

void MainWindow::lock(bool l)
{
     l=!l;
     toolBox->setEnabled(l);
     actionUpdatePalette->setEnabled(l);
     actionPlay->setEnabled(l);
     actionStop->setEnabled(l);
     actionPause->setEnabled(l);
     actionNextFrame->setEnabled(l);
     actionPrevFrame->setEnabled(l);
     actionSetAsStart->setEnabled(l);
     actionSetAsStop->setEnabled(l);
     actionExtractGif->setEnabled(l);

}

QImage MainWindow::finalFrame(long f)
{
     player->seek(f);
     QSize s = player->getCurrentFrame()->size();
     QImage frame = marginBox->isChecked() ? 
	  player->getCurrentFrame()->copy(leftSpin->value(),
					 topSpin->value(),
					 s.width()-leftSpin->value()-rightSpin->value(),
					 s.height()-topSpin->value()-bottomSpin->value()):
	  *player->getCurrentFrame();
     frame = frame.scaled(widthBox->value(),heightBox->value());
     if(redSlider->value() || blueSlider->value() || greenSlider->value())
	  PreviewWidget::applyBalance(&frame,
				      redSlider->value(),
				      greenSlider->value(),
				      blueSlider->value());
     return frame;
}
