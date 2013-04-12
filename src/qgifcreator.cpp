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

#include "qgifcreator.h"
#include <QColor>

QGifCreator::QGifCreator()
{
}

QGifCreator::~QGifCreator()
{
     //clearCache();
}

// Byte* QGifCreator::imageData(const QImage& img)
// {
//      //Byte* frame = new Byte[w*h*3];
//      // for(int i=0;i<w*h*3;i++)
//      // 	  if(i%6)
//      // 	       frame[i] = 255;
//      // 	  else
//      // 	       frame[i] = 0;

//      if(img.isNull())
// 	  return NULL;
//      QImage* i = new QImage(img.mirrored());
//      cache.append(i);
//      return (Byte*)i->bits();
     
//      //return frame;
// }

// void QGifCreator::addFrame(const QImage& img)
// {
//      Byte* b = imageData(img);
//      if(b)
// 	  GifCreator::addFrame(b, duration);
// }

// void QGifCreator::clearCache()
// {
//      qDebug() << "clearing image cache...";
//      while(cache.size())
// 	  delete cache.takeFirst();
// }

void QGifCreator::prepareFrame(QImage* img, ColorMapObject* map)
{

     Frame frame(img->bytesPerLine()*img->height());
     qDebug() << "preparing frame...";
     qDebug() << "bpl*h = " << img->bytesPerLine()*img->height();
     qDebug() << "byte count: " << img->byteCount();
     qDebug() << "w*h*3: " << img->height()*img->width()*3;
     qDebug() << "difference: " << img->byteCount() - (img->height()*img->width()*3);
     const int step = img->format() == QImage::Format_RGB888 ? 3 : 4;
     int f = 0;
     for(int rw=0;rw<img->height();rw++)
     {
	  uchar* data = img->scanLine(rw);
	  for(int i=0;i<img->bytesPerLine();i+=step)
	  {
	       int mi = 0, md = 3 * 256 * 256;
	       for(int j=0;j<map->ColorCount;j++)
	       {
		    int r = map->Colors[j].Red-data[i];
		    int g = map->Colors[j].Green-data[i+1];
		    int b = map->Colors[j].Blue-data[i+2];
		    int d = r*r+g*g+b*b;
		    if(d < md)
		    {
			 md = d;
			 mi = j;
		    }
	       }
	       frame[f] = mi;
	       data[i] = (Byte)map->Colors[mi].Red;
	       data[i+1] = (Byte)map->Colors[mi].Green;
	       data[i+2] = (Byte)map->Colors[mi].Blue;
	       f++;
	  }
     }

/*
//------------ deprecated ---------
     int bytes = img->byteCount();
     Byte* data = img->bits();
     
     for(int i=0,f=0;i<bytes;i+=3,f++)
     {
     	  int mi = 0, md = 3 * 256 * 256;
     	  for(int j=0;j<map->ColorCount;j++)
     	  {
     	       int r = map->Colors[j].Red-data[i];
     	       int g = map->Colors[j].Green-data[i+1];
     	       int b = map->Colors[j].Blue-data[i+2];
     	       int d = r*r+g*g+b*b;
     	       if(d < md)
     	       {
     		    md = d;
     		    mi = j;
     	       }
     	  }
     	  frame[f] = mi;
     	  data[i] = (Byte)map->Colors[mi].Red;
     	  data[i+1] = (Byte)map->Colors[mi].Green;
     	  data[i+2] = (Byte)map->Colors[mi].Blue;			     
     }
*/
     frames.push_back(frame);
     //delay.push_back(duration);
}
