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

#include "previewwidget.h"

PreviewWidget::PreviewWidget(QWidget* parent, Qt::WindowFlags f):
     QWidget(parent,f),smooth(false),imsize(-1,-1),ratio(false),zoom(1)
{
     setFixedSize(480,360);
     show();
     setMouseTracking(true);
}

PreviewWidget::~PreviewWidget()
{
     qDebug() << "preview destroyed...";
}

void PreviewWidget::setImage(const QImage& img, const QSize& size, 
			     bool forceRatio, bool forceSmooth)
{
     if(img.isNull())
	  return;

     if(size.isNull())
     {
	  imsize = img.size();
	  setFixedSize(imsize);
     }
     else
	  imsize = size;

     origSize = img.size();
     image = img;
     // if(!imsize.isValid())
     // 	  setFixedSize(img.size());
     // else
     // {
	  image = image.scaled(
	       imsize*zoom, (ratio || forceRatio) ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio, (
		    (smooth || forceSmooth) ? Qt::SmoothTransformation : Qt::FastTransformation));
      	  //setFixedSize(imsize);
     // }
	  qDebug() << "image size: " << image.size() << ", given size: " << imsize;
	  update();
	  // repaint();
}

void PreviewWidget::clear()
{
     QPixmap p(QSize(1,1));
     p.fill(Qt::transparent);
     setImage(p.toImage());
}

void PreviewWidget::paintEvent(QPaintEvent*)
{
     
     if(image.isNull())
	  return;
     QPainter p(this);
     p.fillRect(this->rect(), Qt::white);
     // int x = (1-zoom)/2*width();
     // int y = (1-zoom)/2*height();
     // x += (width()-image.width())/2;
     // y += (height()-image.height())/2;
     drawnX = (rect().width()-image.width())/2;
     drawnY = (rect().height()-image.height())/2;
     p.drawImage(drawnX, drawnY, image);
     
}

void PreviewWidget::applyCorrection(QImage* img, int h, int s, int v, bool toRGB888, QRect rect)
{
     if(toRGB888 && img->format() != QImage::Format_RGB888)
	  *img = img->convertToFormat(QImage::Format_RGB888);
     //qDebug() << "correction hsv: " << h << "," << s << "," <<v;
     const int step = img->format() == QImage::Format_RGB888 ? 3 : 4;
     //const int step = 3;
     int ch,cs,cv;
     for(int y=0;y<img->height();y++)
     {
	  //qDebug() << "correcting row: " << rw;
	  uchar* data = img->scanLine(y);
	  for(int i=0,x=0;i<img->bytesPerLine();i+=step,x++)
	  {
	       if(!rect.isNull() && !rect.contains(x,y))
		    continue;

	       //qDebug() << "before correction, rgb:" << data[i] << ", " << data[i+1] << ", " <<data[i+2];
	       QColor c(data[i],data[i+1],data[i+2]);
	       c.getHsv(&ch, &cs, &cv);
	       // ch = ch+h > 359 ? 359 : ch+h < 0 ? 0 : ch+h;
	       // cs = cs+s > 255 ? 255 : cs+s < 0 ? 0 : cs+s;
	       // cv = cv+v > 255 ? 255 : cv+v < 0 ? 0 : cv+v;
	       ch = qBound(0,ch+h,359);
	       cs = qBound(0,cs+s,255);
	       cv = qBound(0,cv+v,255);
	       //qDebug() << "correction, hsv:" << ch << ", " << cs << ", " <<cv;

	       c.setHsv(ch,cs,cv);
	     
	       data[i] = c.red();
	       data[i+1] = c.green();
	       data[i+2] = c.blue();
	       

	       //qDebug() << "after correction, rgb:" << data[i] << ", " << data[i+1] << ", " <<data[i+2];
	  }
	  //qDebug() << "row corrected";
     }

     // for(int rw=0;rw<img->height();rw++)
     // {
     // 	  uchar* data = img->scanLine(rw);
     // 	  for(int i=0;i<img->bytesPerLine();i+=step)
     // 	  {
     // 	       data[i] = data[i]+r > 255 ? 255 : data[i]+r < 0 ? 0 : data[i]+r;
     // 	       data[i+1] = data[i+1]+g > 255 ? 255 : data[i+1]+g < 0 ? 0 : data[i+1]+g;
     // 	       data[i+2] = data[i+2]+b > 255 ? 255 : data[i+2]+b < 0 ? 0 : data[i+2]+b;
     // 	  }
     // }


}
