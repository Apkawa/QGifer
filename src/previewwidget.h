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

#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QImage>
#include <QRect>
#include <QDebug>

#define MR_DRAG_PX 3

class PreviewWidget : public QWidget
{
     Q_OBJECT;
public:
     PreviewWidget(QWidget* parent=0, Qt::WindowFlags f=0);
     virtual ~PreviewWidget();
     void setImage(const QImage& img, const QSize& size = QSize(0,0), 
		   bool forceRatio = false, bool forceSmooth = false);
     virtual QImage* getImage() {return &image;}
     const QPoint* getCursorPos() const {return underMouse() ? &cpos : NULL;}
     const double normalizedX() const 
     {return image.isNull() ? -1 : (double)(cpos.x()-((1-zoom)/2*width()))/(double)image.width();}
     const double normalizedY() const 
     {return image.isNull() ? -1 : (double)(cpos.y()-((1-zoom)/2*height()))/(double)image.height();}
     void enableAntialiasing(bool enable) {smooth = enable;}
     virtual void clear();
     void keepAspectRatio(bool keep){ratio = keep;}
     void setZoom(double z) {zoom = z;}
     double getZoom() const {return zoom;}
     static void applyCorrection(QImage* img, int h, int s, int v, 
				 bool toRGB888 = true, QRect rect = QRect());
protected:
     virtual void paintEvent(QPaintEvent*);
     double fit01(double v){if(v<0)return 0;else if(v>1) return 1; else return v;}
     QSize imsize;
     QSize origSize;
     QImage image;
     QPoint cpos;
     bool smooth;
     bool ratio;
     double zoom; //0 - 1
     int drawnX;
     int drawnY;
     private slots:
	  
signals:
     void clicked(double x, double y);
     void marginsChanged();
};

#endif


