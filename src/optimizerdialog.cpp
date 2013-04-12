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

#include "optimizerdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

OptimizerDialog::OptimizerDialog(QSettings* s)
{
     setupUi(this);
     set = s;
     srcEdit->setText(set->value("last_gif","").toString());
     if(srcEdit->text().size()>4)
	  dstEdit->setText(srcEdit->text().insert(srcEdit->text().size()-4,"_optimized"));
     showBox->setChecked(set->value("show_optimizer",false).toBool());
     proc = new QProcess(this);
    
     if(set->value("convert_exec","").toString().isEmpty())
	  set->setValue("convert_exec",findConvert());
     checkIM();

     connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
     connect(srcButton, SIGNAL(clicked()), this, SLOT(setSrc()));
     connect(dstButton, SIGNAL(clicked()), this, SLOT(setDst()));
     connect(imButton, SIGNAL(clicked()), this, SLOT(setIMDir()));
     connect(optButton, SIGNAL(clicked()), this, SLOT(optimize()));
     connect(proc, SIGNAL(finished(int,QProcess::ExitStatus)), 
	     this, SLOT(finished(int,QProcess::ExitStatus)));
     connect(showBox, SIGNAL(stateChanged(int)), 
	     this, SLOT(showStateChanged(int)));

}

OptimizerDialog::~OptimizerDialog()
{
}

void OptimizerDialog::optimize()
{
     if(!QFile::exists(srcEdit->text())){
	  QMessageBox::critical(this, tr("Error"), 
				tr("The chosen source file does not exist!"));
	  return;
     }

     QStringList args;
     args << srcEdit->text() << "-fuzz" << QString::number(fuzzBox->value())+"%" <<
	  "-layers" << "Optimize" << dstEdit->text();
     proc->start(set->value("convert_exec","convert").toString(), args);
     msgLabel->setStyleSheet("color: green; font-size: 11pt");
     msgLabel->setText(tr("Optimizing, please wait..."));
     msgLabel->show();
     srcGroup->setEnabled(false);
     dstGroup->setEnabled(false);
     optButton->setEnabled(false);
     imButton->setEnabled(false);
     qApp->processEvents();
}

void OptimizerDialog::setIMDir()
{
     QString path = QFileDialog::getExistingDirectory(
	  this, tr("Select ImageMagick binary directory"), 
	  qApp->applicationDirPath());
     if(path.isEmpty())
	  return;
     
#if defined(Q_WS_X11)
     path += "/convert";
#elif defined(Q_WS_WIN)
     path += "\\convert.exe";
#endif
     if(!QFile::exists(path))
     {
	  QMessageBox::critical(this, tr("Error"), 
				tr("The chosen directory does not contain \"convert\" executable!"));
	  return;
     }
     
     set->setValue("convert_exec",path);
     checkIM();
}

void OptimizerDialog::setSrc()
{
     QString path = QFileDialog::getOpenFileName(
	  this, tr("Open GIF file"), 
	  set->value("last_gif_dir",qApp->applicationDirPath()).toString(),
	  "GIF files (*.gif)");
     if(!path.isEmpty())
     {
	  srcEdit->setText(path);
	  dstEdit->setText(path.insert(path.size()-4,"_optimized"));
     }
}

void OptimizerDialog::setDst()
{
     QString path = QFileDialog::getSaveFileName(
	  this, tr("Save GIF file"), 
	  set->value("last_gif_dir",qApp->applicationDirPath()).toString(),
	  "GIF files (*.gif)");
     if(!path.isEmpty())
	  dstEdit->setText(path);
}

bool OptimizerDialog::convertAvailable()
{
     return !QProcess::execute(set->value("convert_exec","convert").toString(),
			       QStringList() << "-version");
}

void OptimizerDialog::checkIM()
{
     bool c = convertAvailable();
     if(!c)
     {
	  msgLabel->setStyleSheet("color: red; font-size: 11pt");
	  msgLabel->setText(tr("Error: this operation requires the ImageMagick installed on your system. Please install ImageMagick or choose the directory with \"convert\" executable file."));
     }
     msgLabel->setVisible(!c);
     optButton->setEnabled(c);
     srcGroup->setEnabled(c);
     dstGroup->setEnabled(c);
}

void OptimizerDialog::finished(int,QProcess::ExitStatus status)
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
     optButton->setEnabled(true);
     imButton->setEnabled(true);
}

void OptimizerDialog::showStateChanged(int s)
{
     set->setValue("show_optimizer", s == Qt::Checked);
}

QStringList OptimizerDialog::sysEnv()
{
     QString s(getenv("PATH"));
#if defined(Q_WS_X11)
     return s.split(":",QString::SkipEmptyParts);
#elif defined(Q_WS_WIN)
     return s.split(";",QString::SkipEmptyParts);
#endif
}

QString OptimizerDialog::findConvert()
{
     QString exec =
#if defined(Q_WS_X11)
	  "/convert";
#elif defined(Q_WS_WIN)
     "\\convert.exe";
#endif
     QStringList env = sysEnv();
     qDebug() << "sys env: " << env;
     for(int i=0;i<env.size();i++)
	  if(QFile::exists(env.at(i)+exec))
	  {
	       qDebug() << "checking: " << env.at(i)+exec;
	       return env.at(i)+exec;
	  }
     return "";
}
