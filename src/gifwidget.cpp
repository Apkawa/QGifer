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

#include "gifwidget.h"
#include <QProgressDialog>
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>

GifWidget::GifWidget(QSettings* s): timerId(-1), currentFrame(-1)
{
     setupUi(this);
     set = s;
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
     a->setIcon(QIcon(":/res/save.png"));
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
	  skipped = 0;
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

     QString filename = QFileDialog::getSaveFileName(
	  this, tr("Save GIF file"), 
	  set->value("last_gif_dir","").toString()+
	  (suggestedName.isEmpty() ? "" : "/"+suggestedName+".gif"),
	  "GIF files (*.gif);;All files (*.*)");

     if(filename.isEmpty())
	  return;

     if(!prevFrames.size())
	  return;
     saveGif(filename);
}

void GifWidget::saveGif(const QString& filename)
{
     pause();
     gif.setDuration((double)intervalBox->value()/1000);
     if(!gif.save(filename.toStdString().c_str(),
		  saveEveryBox->isChecked() ? seBox->value() : 1))
	  QMessageBox::critical(this,tr("Error"),tr("Unexpected error while saving GIF file!"));

     emit gifSaved(filename);
}

void GifWidget::timerEvent(QTimerEvent*)
{
     preview->setImage(prevFrames.at(currentFrame));
     currentFrame += saveEveryBox->isChecked() ? seBox->value() : 1;

     if(currentFrame >= prevFrames.size())
	  currentFrame = skipped = 0;
}



