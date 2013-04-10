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

     if(checkFFMPEG())
	  connect(actionConverter, SIGNAL(triggered()), this, SLOT(runConverter()));
     else
	  actionConverter->setVisible(false);

     connect(actionAbout, SIGNAL(triggered()), this, SLOT(about()));
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
     connect(widthBox, SIGNAL(valueChanged(int)), this, SLOT(outputWidthChanged(int)));
     connect(heightBox, SIGNAL(valueChanged(int)), this, SLOT(outputHeightChanged(int)));

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

     connect(getWHButton, SIGNAL(clicked()), this, SLOT(estimateOutputSize()));
     connect(whRatioBox, SIGNAL(stateChanged(int)),this, SLOT(whRatioChanged(int)));
     //marginesy
     connectMargins();

     connect(player->previewWidget(), SIGNAL(marginsChanged()), this, SLOT(marginsChanged()));
     //test
     //openVideo("/home/chodak/rec/tkw540.avi");
     loadSettings();
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
	  set->value("last_video_dir","").toString(),
	  "Video files (*.avi *.mp4 *.mpg *.ogv);;All files (*.*)");
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
	 vidfile = path;
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

     //--- korekcja rozdzielczosci, przy niektorych wartosciach wystepuje bug
     //--- i gif jest przekrzywiony. Liczba byteCount() jest większa od w*h*3
     //--- o wielokrotnosc h... wyrownamy wiec roznice na podstawie pierwszej klatki
     if(heightBox->value()%2) heightBox->setValue(heightBox->value()+1);
     qDebug() << "correcting resolution....";
     QImage testframe = finalFrame(startBox->value());
     int diff = testframe.byteCount()-(testframe.width()*testframe.height()*3);
     qDebug() << "byte difference: " << diff;
     widthBox->setValue(widthBox->value()+diff/heightBox->value());
     qApp->processEvents();
     qDebug() << "corrected.";

     GifWidget* g = new GifWidget(set);
     connect(g, SIGNAL(gifSaved(const QString&)), this, SLOT(gifSaved(const QString&)));
     g->setAttribute(Qt::WA_DeleteOnClose, true);
     g->setPalette(paletteWidget->map());
     
     QString sn = QFileInfo(vidfile).baseName()+"_"+
		    QString::number(startBox->value())+"-"+
	  QString::number(stopBox->value());
     g->suggestName(sn);
     g->setWindowTitle(sn);
     QProgressDialog pd("Rendering frames...", "Abort", startBox->value(), 
			      stopBox->value(), this);
     pd.setWindowModality(Qt::WindowModal);
     pd.show();
     qApp->processEvents();
     for(long i=startBox->value();i<=stopBox->value() && !pd.wasCanceled();i++)
     {
	  pd.setValue(i);
	  g->addFrame(finalFrame(i));
     }
     pd.setValue(stopBox->value());


     g->show();
     g->move(x()+width()/2-g->minimumSize().width()/2,y()+height()/2-g->minimumSize().height()/2);
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
     if(autoPaletteBox->isChecked()){
	  player->seek(v);
	  updatePalette();
     }
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
     applyMargins();
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
     return frame.convertToFormat(QImage::Format_RGB888);
}

void MainWindow::loadSettings()
{
     zoomSlider->setValue(set->value("zoom",100).toInt());
     smoothBox->setChecked(set->value("smooth_preview",false).toBool());
     ratioBox->setChecked(set->value("keep_ratio",false).toBool());
     switch(set->value("loop",0).toInt())
     {
     case 1: laRadio->setChecked(true);break;
     case 2: ssRadio->setChecked(true); break;
     default: nlRadio->setChecked(true);
     }
     marginBox->setChecked(set->value("use_margins",true).toBool());
     // leftSpin->setValue(set->value("left_margin",10).toInt());
     // rightSpin->setValue(set->value("right_margin",10).toInt());
     // topSpin->setValue(set->value("top_margin",10).toInt());
     // bottomSpin->setValue(set->value("bottom_margin",10).toInt());
     widthBox->setValue(set->value("output_width",320).toInt());
     heightBox->setValue(set->value("output_height",240).toInt());
     paletteBox->setValue(set->value("palette_size",10).toInt());
     autoPaletteBox->setChecked(set->value("auto_palette",false).toBool());
     whRatioBox->setChecked(set->value("wh_ratio",false).toBool());
}

void MainWindow::saveSettings()
{
     set->setValue("zoom",zoomSlider->value());
     set->setValue("smooth_preview",smoothBox->isChecked());
     set->setValue("keep_ratio",ratioBox->isChecked());
     set->setValue("loop",(laRadio->isChecked() ? 1 : ssRadio->isChecked() ? 2 : 0));
     set->setValue("use_margins",marginBox->isChecked());
     // set->setValue("left_margin",leftSpin->value());
     // set->setValue("right_margin",rightSpin->value());
     // set->setValue("top_margin",topSpin->value());
     // set->setValue("bottom_margin",bottomSpin->value());
     set->setValue("output_width",widthBox->value());
     set->setValue("output_height",heightBox->value());
     set->setValue("palette_size",paletteBox->value());
     set->setValue("auto_palette",autoPaletteBox->isChecked());
     set->setValue("wh_ratio",whRatioBox->isChecked());
     
}

void MainWindow::estimateOutputSize()
{
     QSize s = player->getCurrentFrame()->size();
     if(marginBox->isChecked())
     {
	  s.setWidth(s.width()-leftSpin->value()-rightSpin->value());
	  s.setHeight(s.height()-topSpin->value()-bottomSpin->value());	  
     }
     float tmp = whRatio;
     whRatio = -1;
     widthBox->setValue(s.width());
     heightBox->setValue(s.height());
     whRatio = tmp;
}


void MainWindow::outputWidthChanged(int v)
{
     if(!whRatioBox->isChecked() || whRatio<=0)
	  return;
     heightBox->disconnect();
     heightBox->setValue(ceil(v/whRatio));
     connect(heightBox, SIGNAL(valueChanged(int)), this, SLOT(outputHeightChanged(int)));
}

void MainWindow::outputHeightChanged(int v)
{
     if(!whRatioBox->isChecked() || whRatio<=0)
	  return;
     widthBox->disconnect();
     widthBox->setValue(ceil(v*whRatio));
     connect(widthBox, SIGNAL(valueChanged(int)), this, SLOT(outputWidthChanged(int)));

}

void MainWindow::whRatioChanged(int s)
{
     if(s == Qt::Checked)
	  whRatio = (float)widthBox->value()/(float)heightBox->value();
}

void MainWindow::startChanged(int v)
{
     multiSlider->setPosA(v,false);
     if(autoPaletteBox->isChecked()){
	  player->seek(v);
	  updatePalette();
     }
}
