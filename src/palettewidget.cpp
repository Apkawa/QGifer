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
#include "qgifcreator.h"

PaletteWidget::PaletteWidget(QWidget* parent, Qt::WindowFlags f): 
     QWidget(parent,f), cols(10), palette(NULL), size(256), sqSize(0)
{
     // for(int i=0;i<257;i++)
     // {
     // 	  candidates[i].Red = 255-i%256;
     // 	  candidates[i].Green = 255-i/256;
     // 	  candidates[i].Blue = 255;
     // }
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
	  if(++c >= cols)
	  {
	       c = 0;
	       r++;
	  }
     }
     
}

bool PaletteWidget::fromImage(const QImage& img, int palette_size, bool delPrev, float mindiff)
{

     if(palette && delPrev)
     {
	  FreeMapObject(palette);
	  palette = NULL;
     }

     size = palette_size;
     QImage fimg = img.mirrored().convertToFormat(QImage::Format_RGB888);
     Byte* data = fimg.bits();

     int npix = img.width()*img.height();
     Frame r(npix),g(npix),b(npix);
     Frame output(npix);
     for (int i=0, j=0; i<npix; i++)
     {
	  r[i]=data[j++];
	  g[i]=data[j++];
	  b[i]=data[j++];
     }
     
     ColorMapObject* previous = palette;
     palette = MakeMapObject(size, NULL);
     if (!palette) 
     {
	  qDebug() << "NULL palette!";
	  return false;
     }
     
     //quantize(fimg,palette);

     // qDebug() << "\nfirst palette: ";
     // for(int i=0;i<palette->ColorCount;i++)
     // 	  qDebug() << i << ": " << palette->Colors[i].Red << ", "  << palette->Colors[i].Green << ", "  << palette->Colors[i].Blue;

     if (QuantizeBuffer(img.width(), img.height(), &size, 
     			&(r[0]),&(g[0]),&(b[0]), &(output[0]), 
     			palette->Colors) == GIF_ERROR) 
     {
     	  qDebug() << "quentize failed!";
     	  palette = NULL;
     	  return false; 
     }

     float df = difference(palette,previous);
     qDebug() << "difference: " << df;
     if(previous && df < mindiff)
     {
	  FreeMapObject(palette);
	  palette = previous;
     }

     // qDebug() << "\nsecond palette: ";
     // for(int i=0;i<palette->ColorCount;i++)
     // 	  qDebug() << i << ": " << palette->Colors[i].Red << ", "  << palette->Colors[i].Green << ", "  << palette->Colors[i].Blue;

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

// float PaletteWidget::distance(const GifColorType& a, const GifColorType& b)
// {
//      GifColorType c;
//      c.Red = b.Red-a.Red;
//      c.Green = b.Green-a.Green;
//      c.Blue = b.Blue-a.Blue;
//      return sqrtf(c.Red*c.Red+c.Green*c.Green+c.Blue*c.Blue)/RGB_MAXD;
// }

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

// int PaletteWidget::quantize(const QImage& img, ColorMapObject* map)
// {
//      qDebug() << "quantize...";

//      long npix = img.width()*img.height();
//      const Byte* data = img.bits();
//      // qDebug() << "first pixel: " << r[0] << ", " << g[0] << ", " << b[0];
//      // qDebug() << "last pixel: " << r[npix-1] << ", " << g[npix-1] << ", " << b[npix-1];

//      QList<ColorItem> colors;
     
//      for(int i=0;i<npix;i+=3)
//      {
//      	  // qDebug() << i/3 << " pixel: " << data[i] << ", " << data[i+1] << ", " << data[i+2];
//      	  GifColorType pix;
//      	  pix.Red = data[i];
//      	  pix.Green = data[i+1];
//      	  pix.Blue = data[i+2];
//      	  int pi = findPixel(colors,pix);
// 	  if(pi != -1)
// 	  {
// 	       colors[pi].count++;
// 	       // if(pi > 0)
// 	       // 	    while(colors[pi].count > colors[pi-1].count)
// 	       // 	    {
// 	       // 		 //qDebug() << "swapping colors with count...";
// 	       // 		 colors.swap(pi,pi-1);
// 	       // 		 pi--;
// 	       // 		 if(!pi)
// 	       // 		      break;
// 	       // 	    }
// 	  }
// 	  else
// 	  {
// 	       //qDebug() << "adding new color with count...";
// 	       ColorItem cc;
// 	       cc.color = pix;
// 	       cc.count = 0;
// 	       colors.append(cc);
// 	  }
//      }

//      qDebug() << "colors size: " << colors.size();
//      qDebug() << "most popular pixel: " << colors.at(0).color.Red << ", " << colors.at(0).color.Green << ", " << colors.at(0).color.Blue;
//      //qDebug() << "phash size: " << phash.size();

//      if(colors.size() < map->ColorCount)
// 	  map->ColorCount = colors.size();

//      //colors[0] = newColor(colors);
//      reduceTo(&colors,map->ColorCount);
//      qDebug() << "copying color map...";
//      for(int i=0;i<map->ColorCount;i++)
// 	  map->Colors[i] = colors[i].color;

//      return GIF_OK;
// }

// int PaletteWidget::findPixel(const QList<ColorItem>& list, const GifColorType& c)
// {
//      for(int i=list.size()-1;i>=0;i--)
// 	  if(list[i].color.Blue == c.Blue && list[i].color.Red == c.Red && 
// 	     list[i].color.Green == c.Green)
// 	       return i;
//      return -1;
// }

// GifColorType PaletteWidget::newColor(const QList<ColorItem>& list)
// {
//      GifColorType c;

//      int s = list.size() > 256 ? 256 : list.size();
//      for(int i=0;i<257;i++)
// 	  for(int j=0;j<s;j++)
// 	       if(list[j].color.Red != candidates[i].Red &&
// 		  list[j].color.Green != candidates[i].Green &&
// 		  list[j].color.Blue != candidates[i].Blue)
// 		    return candidates[j];
//      return c;
// }

// void PaletteWidget::fillNearests(QList<ColorItem>* list)
// {
//      qDebug() << "filling nearests...";
//      for(int i=0;i<list->size()-1;i++)
//      {
// 	  ColorItem* ca = &(*list)[i];
// 	  if(ca->nearest == -2)
// 	       continue;
// 	  float mind = RGB_MAXD;
// 	  int minj = 0;
// 	  for(int j=i+1;j<list->size();j++)
// 	  {
// 	       ColorItem* cb = &(*list)[j];
// 	       float d = distance((const GifColorType&)*ca,(const GifColorType&)*cb);
// 	       if(d < mind)
// 	       {
// 		    mind = d;
// 		    minj = j;
// 	       }
// 	  }
// 	  //qDebug() << "minj for " << i << ": " << minj;
// 	  //qDebug() << "distance: " << mind;
// 	  ca->nearest = minj;
// 	  ca->dist = mind;
// 	  (*list)[minj].nearest = i;
// 	  (*list)[minj].dist = mind;
// 	  if(i>0)
// 	  {
// 	       int p = i;
// 	       while(list->at(p).dist < list->at(p-1).dist)
// 	       {
// 		    list->swap(p,p-1);
// 		    p--;
// 		    if(!p)
// 			 break;
// 	       }
// 	  }
// 	  //qDebug() << "mind: " << mind;
	       
//      }
// }

// void PaletteWidget::reduceTo(QList<ColorItem>* list, int limit)
// {
//      fillNearests(list);
//      int ignored = 0;
//      int index = 0;
//      while(list->size()-ignored > limit)
//      {
	  
// 	  ColorItem* ci = &(*list)[index];
// 	  if(ci->nearest != -2)
// 	       (*list)[ci->nearest].nearest = -2;
// 	  ignored++;
// 	  index++;
// 	  //qDebug() << "checking " << index << " with dist: " << ci->dist;
// 	  // if(// ci->nearest != -2 && 
// 	  //    list->at(ci->nearest).nearest != -2)
// 	  // {
// 	  //     ci->nearest = -2;
// 	  //  ignored++;
// 	       //qDebug() << "ignored: " << ignored;
// 	  // }
// 	  //index--;
// 	  //qDebug() << "current reduction index: " << index;
// 	  if(index == list->size())
// 	  {
// 	       //index = list->size()-1; //moze size()-1-ignored?
// 	       index = 0;
// 	       fillNearests(list);
// 	  }
//      }

//      qDebug() << "removing ignored colors...";
//      for(int i=0;i<list->size();i++)
// 	  if(list->at(i).nearest == -2)
// 	  {
// 	       list->removeAt(i);
// 	       i--;
// 	  }
//      qDebug() << "..done!";
// }

// int PaletteWidget::findMinDistIndex(QList<ColorItem>* list)
// {
//      //qDebug() << "finding min dist index...";
//      float mind = RGB_MAXD;
//      int min = 0;
//      for(int i=0;i<list->size();i++)
// 	  if(list->at(i).nearest != -2 && list->at(i).dist < mind)
// 	  {
// 	       min = i;
// 	       mind = list->at(i).dist;
// 	  }
//      // qDebug() << "min distance in set: " << mind << ", index: " << min;
//      return min;
// }
