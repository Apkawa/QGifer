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

#include "palettewidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QColorDialog>
#include <QFile>
#include <QTextStream>
#include "qgifcreator.h"

PaletteWidget::PaletteWidget(QWidget* parent, Qt::WindowFlags f): 
     QWidget(parent,f), cols(10), palette(NULL), size(256), sqSize(0),
     hlIndex(-1)
{
     setMouseTracking(true);
}

PaletteWidget::~PaletteWidget()
{
     FreeMapObject(palette);
}

void PaletteWidget::paintEvent(QPaintEvent*)
{
     if(!palette)
	  return;
     if(!palette->ColorCount)
	  return;
     QPainter p(this);
     
     int r=0,c=0;
     p.drawRect(0,0,sqSize*cols+1,sqSize*(size/cols)+1+sqSize);
     for(int i=0;i<size;i++)
     {
	  p.fillRect(1+c*sqSize,1+r*sqSize,sqSize,sqSize,QColor(
		     palette->Colors[i].Red,
		     palette->Colors[i].Green,
		     palette->Colors[i].Blue));
	  if(++c >= cols){
	       c = 0;
	       r++;
	  }
     }
     
     //powiekszenie podswietlonego
     if(hlIndex != -1)
     {
	  QRect r(1+(hlIndex%cols)*sqSize-5,1+(hlIndex/cols)*sqSize-5,sqSize+10,sqSize+10);
	  p.fillRect(r,QColor(
		     palette->Colors[hlIndex].Red,
		     palette->Colors[hlIndex].Green,
		     palette->Colors[hlIndex].Blue));
	  p.drawRect(r);
     }

}

void PaletteWidget::mouseReleaseEvent(QMouseEvent* e)
{
     if(hlIndex == -1 || hlIndex >= size || !palette)
	  return;

     int i = hlIndex; //hlIndex zmieni sie w czasie wyswietlania dialogu
     QColor c = QColorDialog::getColor(QColor(
					    palette->Colors[i].Red,
					    palette->Colors[i].Green,
					    palette->Colors[i].Blue));

     if(c.isValid())
     {
     	  palette->Colors[i].Red = c.red();
     	  palette->Colors[i].Green = c.green();
     	  palette->Colors[i].Blue = c.blue();
     }
}

void PaletteWidget::mouseMoveEvent(QMouseEvent* e)
{
     if(!sqSize || !palette)
	  return;
     int x = (e->x()-1)/sqSize;
     int y = (e->y()-1)/sqSize;

     if(x < 0 || y < 0 || x >= cols || y > size/cols || cols*y+x >= size)
     {
	  hlIndex = -1;
	  setCursor(Qt::ArrowCursor);
     }
     else
     {
	  hlIndex = cols*y+x;
	  setCursor(Qt::PointingHandCursor);
     }
     update();
}

bool PaletteWidget::fromImage(const QImage& img, int palette_size, float mindiff)
{

     if(palette && mindiff > 1)
     {
	  qDebug() << "deleting old palette, size: " << size << ", colors: " << palette->ColorCount;
	  FreeMapObject(palette);
	  qDebug() << "done";
	  palette = NULL;
     }

     size = palette_size;
     QImage fimg = img.mirrored().convertToFormat(QImage::Format_RGB888);
     


     const int npix = fimg.width()*fimg.height();
     const int bytesPerLine = fimg.width()*3; //wynik INNY niz fimg.bytesPerLine()
     Frame r(npix),g(npix),b(npix);
     Frame output(npix);

     //Byte* data = fimg.bits();
     // for (int i=0, j=0; i<npix; i++)
     // {
     // 	  r[i]=data[j++];
     // 	  g[i]=data[j++];
     // 	  b[i]=data[j++];
     // }

     int cpix = 0;
     
     for(int rw=0;rw<fimg.height();rw++)
     {
	  uchar* line = fimg.scanLine(rw);
	  //for(int i=0;i+2<fimg.bytesPerLine();i+=3)
	  for(int i=0;i+2<bytesPerLine;i+=3)
	  {
	       //qDebug() << "cpix: " << cpix;
	       r[cpix]=line[i];
	       g[cpix]=line[i+1];
	       b[cpix]=line[i+2];
	       cpix++;
	  }
     }

     qDebug() << "ostatni uzyty cpix: " << cpix-1;

     ColorMapObject* previous = palette;
     palette = MakeMapObject(size, NULL);
     if (!palette) 
     {
	  qDebug() << "NULL palette!";
	  return false;
     }
     

     if (QuantizeBuffer(fimg.width(), fimg.height(), &size, 
     			&(r[0]),&(g[0]),&(b[0]), &(output[0]), 
     			palette->Colors) == GIF_ERROR) 
     {
     	  qDebug() << "quentize failed!";
     	  palette = NULL;
     	  return false; 
     }

     float df = difference(palette,previous);
     //qDebug() << "difference: " << df;
     if(previous && df < mindiff)
     {
	  FreeMapObject(palette);
	  palette = previous;
     }
     else if(df >= mindiff)
	  FreeMapObject(previous);

     // qDebug() << "palette (" << palette->ColorCount << ") :";
     // for(int i=0;i<size;i++)
     // 	  qDebug() << i << ": (" << palette->Colors[i].Red << "," << 
     // 	       palette->Colors[i].Green << "," << 
     // 	       palette->Colors[i].Blue << ")"; 
	       
     //setFixedSize(width(), (size/cols)*((width()-2)/cols));
     setFixedSize(width(),(size/cols)*sqSize+sqSize+2);
     update();
     return true;
}


float PaletteWidget::difference(ColorMapObject* a, ColorMapObject* b)
{
     if(!a || !b)
	  return 0;
     int same = 0;
     for(int c=0; c < a->ColorCount; c++)
     {
	  GifColorType& cc = a->Colors[c];
	  for(int p=0; p < b->ColorCount; p++)
	  {
	       GifColorType& pc = b->Colors[p];
	       if(pc.Red == cc.Red && pc.Green == cc.Green && pc.Blue == cc.Blue)
		    same++;
	  }
     }
     return (float)same/(float)size;
}


QString PaletteWidget::toString()
{
     QString str;
     for(int i=0;i<palette->ColorCount;i++)
	  str += QString::number(palette->Colors[i].Red) + ","
	       + QString::number(palette->Colors[i].Green) + ","
	       + QString::number(palette->Colors[i].Blue) + ";";
     return str;
}

bool PaletteWidget::fromString(const QString& str)
{
     QStringList rgb = QString(str).split(";", QString::SkipEmptyParts);
     if(palette)
	  FreeMapObject(palette);
     palette = MakeMapObject(rgb.size(), NULL);
     if(!palette)
	  return false;
     size = rgb.size();
     qDebug() << "loaded palette size: " << size;
     for(int i=0;i<size;i++)
     {
	  QStringList c = rgb.at(i).split(",");
	  if(c.size() != 3)
	       return false;
	  palette->Colors[i].Red = c.at(0).toInt();
	  palette->Colors[i].Green = c.at(1).toInt();
	  palette->Colors[i].Blue = c.at(2).toInt();
     }
     setFixedSize(width(),(size/cols)*sqSize+sqSize+2);
     update();
     return true;
}

bool PaletteWidget::toFile(const QString& path)
{
     if(!palette)
	  return false;
     QFile file(path);
     if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
         return false;

     QTextStream out(&file);
     out << toString();
     file.close();

     return true;
}

bool PaletteWidget::fromFile(const QString& path)
{
     QFile file(path);
     if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
         return false;
     
     return fromString(file.readAll());
}

