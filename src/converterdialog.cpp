#include "converterdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTime>
#include <QDebug>

ConverterDialog::ConverterDialog(QSettings* s)
{
     setupUi(this);
     set = s;
     proc = new QProcess(this);
     msgLabel->hide();
     connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
     connect(srcButton, SIGNAL(clicked()), this, SLOT(setSrc()));
     connect(dstButton, SIGNAL(clicked()), this, SLOT(setDst()));
     connect(convertButton, SIGNAL(clicked()), this, SLOT(convert()));
     connect(proc, SIGNAL(finished(int,QProcess::ExitStatus)), 
	     this, SLOT(finished(int,QProcess::ExitStatus)));
}

ConverterDialog::~ConverterDialog()
{
     
}

void ConverterDialog::convert()
{
     QTime z(0,0,0);
     int start = z.secsTo(fromEdit->time());
     int length = fromEdit->time().secsTo(toEdit->time());

     qDebug() << "start: " << start;
     qDebug() << "length: " << length;

     if(length<1){
	  QMessageBox::critical(this, tr("Error"), 
				tr("The length is invalid!"));
	  return;
     }

     if(!QFile::exists(srcEdit->text())){
	  QMessageBox::critical(this, tr("Error"), 
				tr("The chosen source file does not exist!"));
	  return;
     }

     QStringList args;
     args << "-i" << srcEdit->text() << "-y" << "-sameq" << "-vcodec" << "mjpeg" << 
	  "-ss" << QString::number(start) << "-t" << QString::number(length) << dstEdit->text();
     proc->start("ffmpeg", args);
     msgLabel->setStyleSheet("color: green; font-size: 11pt");
     msgLabel->setText(tr("Converting, please wait..."));
     msgLabel->show();
     srcGroup->setEnabled(false);
     dstGroup->setEnabled(false);
     convertButton->setEnabled(false);
     qApp->processEvents();
}

void ConverterDialog::setSrc()
{
     QString path = QFileDialog::getOpenFileName(
	  this, tr("Open video file"), 
	  set->value("last_video_dir",qApp->applicationDirPath()).toString(),
	  "video files (*.avi *.mp4 *.flv *.mpg);;all files(*)");
     if(!path.isEmpty())
     {
	  srcEdit->setText(path);
	  dstEdit->setText(path.left(path.size()-4)+"_mjpeg.avi");
     }
}

void ConverterDialog::setDst()
{
     QString path = QFileDialog::getSaveFileName(
	  this, tr("Save AVI file"), 
	  set->value("last_video_dir",qApp->applicationDirPath()).toString(),
	  "AVI files (*.avi)");
     if(!path.isEmpty())
	  dstEdit->setText(path);
}

void ConverterDialog::finished(int,QProcess::ExitStatus status)
{
     if(status == QProcess::NormalExit && QFile::exists(dstEdit->text()))
     {
	  msgLabel->setStyleSheet("color: green; font-size: 11pt");
	  msgLabel->setText(tr("Done!"));
     }
     else if(status == QProcess::NormalExit)
     {
	  msgLabel->setStyleSheet("color: red; font-size: 11pt");
	  msgLabel->setText(tr("Can not write to destination file! (access denied?)"));
     }
     else
     {
	  msgLabel->setStyleSheet("color: red; font-size: 11pt");
	  msgLabel->setText(tr("Error: ")+QString(proc->readAllStandardError()));
     }
     msgLabel->show();
     srcGroup->setEnabled(true);
     dstGroup->setEnabled(true);
     convertButton->setEnabled(true);
}
