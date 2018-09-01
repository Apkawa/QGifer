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

#ifndef PALETTEWIDGET_H
#define PALETTEWIDGET_H

#include <QWidget>
#include <gif_lib.h>

class PaletteWidget : public QWidget
{
     Q_OBJECT;
public:
     PaletteWidget(QWidget* parent=0, Qt::WindowFlags f=0);
     virtual ~PaletteWidget();
     ColorMapObject* map() {return palette;}
     ColorMapObject* mapCopy() {return GifMakeMapObject(palette->ColorCount, palette->Colors);}
     bool fromImage(const QImage& img, int palette_size, float mindiff = 2);
     void setColumnCount(int cc){cols = cc;}
     bool toFile(const QString& path);
     bool fromFile(const QString& path);
     QString toString();
     bool fromString(const QString& str);
     int getSize() const {return size;}
     void clear() {if(palette) GifFreeMapObject(palette); palette = NULL; update();}
private:
     int size;
     int cols;
     int sqSize;
     int hlIndex;
     ColorMapObject* palette;
     float difference(ColorMapObject* a, ColorMapObject* b);
     void paintEvent(QPaintEvent*);
     void resizeEvent(QResizeEvent*){sqSize = (width()-10)/cols;}
     void mouseReleaseEvent(QMouseEvent*);
     void mouseMoveEvent(QMouseEvent*);
     void leaveEvent(QEvent*){hlIndex=-1;}
     private slots:

};

#endif
