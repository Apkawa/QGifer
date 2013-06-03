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

#include <QColor>
#include <QApplication>
#include "qgifcreator.h"

QGifCreator::QGifCreator():progressDialog(NULL)
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

void QGifCreator::prepareFrame(QImage* img, ColorMapObject* map, bool dither)
{
     if(!map && cmaps.size())
	  map = cmaps.at(cmaps.size()-1);
     else if(!cmaps.size())
	  return;
	  
     //Frame frame(img->bytesPerLine()*img->height());
     

     // qDebug() << "preparing frame...";
     // qDebug() << "bpl*h = " << img->bytesPerLine()*img->height();
     // qDebug() << "byte count: " << img->byteCount();
     // qDebug() << "w*h*3: " << img->height()*img->width()*3;
     // qDebug() << "difference: " << img->byteCount() - (img->height()*img->width()*3);

     const int step = img->format() == QImage::Format_RGB888 ? 3 : 4;
     const int npix = img->width()*img->height();
     const int bytesPerLine = img->width()*step; //wynik INNY niz img->bytesPerLine()
     Frame frame(npix);

     int f = 0;
     for(int rw=0;rw<img->height();rw++)
     {
	  uchar* line = img->scanLine(rw);
	  //for(int i=0;i<img->bytesPerLine();i+=step)
	  for(int i=0;i+2<bytesPerLine;i+=step)
	  {
	       int mi = 0, md = 3 * 256 * 256;
	       for(int j=0;j<map->ColorCount;j++)
	       {
		    int r = map->Colors[j].Red-line[i];
		    int g = map->Colors[j].Green-line[i+1];
		    int b = map->Colors[j].Blue-line[i+2];
		    int d = r*r+g*g+b*b;
		    if(d < md)
		    {
			 md = d;
			 mi = j;
		    }
	       }
	       frame[f] = mi; //indeks najbliższego koloru

	       //dither = false;
	       if(dither)
	       {
		    //qDebug() << "dithering...";
		    int old[3] = {line[i], line[i+1], line[i+2]};
		    //przypisujemy kolor z palety:
		    line[i] = (Byte)map->Colors[mi].Red;
		    line[i+1] = (Byte)map->Colors[mi].Green;
		    line[i+2] = (Byte)map->Colors[mi].Blue;
		    int qerror[3] = {old[0]-line[i], old[1]-line[i+1], old[2]-line[i+2]};

		    //qDebug() << "qerror: " << qerror[0] << qerror[1] << qerror[2];
		    //qDebug() << "7/16*qerror: " << (float)7/16*qerror[0] << (float)7/16*qerror[1] << 7/16*qerror[2];

		    //prawy
		    if(i+2+step < img->bytesPerLine())
		    {
			 line[i+step] = 
			      qBound(0,(int)(line[i+step]+(float)7/16*qerror[0]),255);
			 line[i+1+step] = 
			      qBound(0,(int)(line[i+1+step]+(float)7/16*qerror[1]),255);
			 line[i+2+step] = 
			      qBound(0,(int)(line[i+2+step]+(float)7/16*qerror[2]),255);

			 //prawy dolny
			 if(rw+1<img->height())
			 {
			      uchar* nextLine = img->scanLine(rw+1);
			      //qDebug() << "prawy dolny przed ditheringiem: " << nextLine[i+step] << nextLine[i+step+1] << nextLine[i+step+2];

			      nextLine[i+step] = 
			      	   qBound(0,(int)(nextLine[i+step]+(float)1/16*qerror[0]),255);
			      nextLine[i+1+step] = 
			      	   qBound(0,(int)(nextLine[i+1+step]+(float)1/16*qerror[1]),255);
			      nextLine[i+2+step] = 
			      	   qBound(0,(int)(nextLine[i+2+step]+(float)1/16*qerror[2]),255);

			      //qDebug() << "prawy dolny po ditheringu: " << nextLine[i+step] << nextLine[i+step+1] << nextLine[i+step+2];
			 }
		    }
		    //dolny
		    if(rw+1<img->height())
		    {
			 uchar* nextLine = img->scanLine(rw+1);
			 nextLine[i] = 
			      qBound(0,(int)(nextLine[i+step]+(float)5/16*qerror[0]),255);
			 nextLine[i+1] = 
			      qBound(0,(int)(nextLine[i+1]+(float)5/16*qerror[1]),255);
			 nextLine[i+2] = 
			      qBound(0,(int)(nextLine[i+2]+(float)5/16*qerror[2]),255);

			 //lewy dolny
			 if(i-step > 0)
			 {
			      nextLine[i-step] = 
			      	   qBound(0,(int)(nextLine[i-step]+(float)3/16*qerror[0]),255);
			      nextLine[i+1-step] = 
				   qBound(0,(int)(nextLine[i+1-step]+(float)3/16*qerror[1]),255);
			      nextLine[i+2-step] = 
				   qBound(0,(int)(nextLine[i+2-step]+(float)3/16*qerror[2]),255);
			 }
		    }
	       }
	       else
	       {
		    //przypisujemy kolor z palety:
		    line[i] = (Byte)map->Colors[mi].Red;
		    line[i+1] = (Byte)map->Colors[mi].Green;
		    line[i+2] = (Byte)map->Colors[mi].Blue;
	       }
	       
	       f++;
	  }
     }

     frames.push_back(frame);
     //delay.push_back(duration);
}

bool QGifCreator::savingProgress(int p)
{
     if(!progressDialog){
	  progressDialog = new QProgressDialog(
	       tr("Saving GIF file..."), tr("Cancel"), 0, 100, NULL);
	  progressDialog->setWindowModality(Qt::WindowModal);
	  progressDialog->setAutoClose(false);
	  progressDialog->setAutoReset(false);
     }
     progressDialog->setValue(p);
     qApp->processEvents();
     return !progressDialog->wasCanceled();
}
