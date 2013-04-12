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

#ifndef GIFWIDGET_H
#define GIFWIDGET_H

#include <QWidget>
#include <QList>
#include <QSettings>
#include "qgifcreator.h"
#include "ui_gifwidget.h"

class GifWidget : public QWidget, public Ui::GifWidget
{
     Q_OBJECT;
public:
     GifWidget(QSettings* s);
     virtual ~GifWidget();
     void addFrame(const QImage& f);
     void setPalette(ColorMapObject* map, int res){palette = map; gif.setPalette(map, res);}
     void suggestName(const QString& name){suggestedName = name;}
     void saveGif(const QString& gif);
signals:
     void gifSaved(const QString&);

private:
     QGifCreator gif;
     ColorMapObject* palette;
     QSettings* set;
     void createActions();
     QString suggestedName;
     QList<QImage> prevFrames;
     int timerId;
     int currentFrame;
     int skipped;
     void timerEvent(QTimerEvent*);
     public slots:
	  void save();
	  void play();
	  void pause();
	  void adjustWidgetSize(){adjustSize();}
};

#endif
