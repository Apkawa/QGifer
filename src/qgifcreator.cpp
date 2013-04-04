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
     // for(int i=0;i<img->width();i++)
     // 	  for(int j=0;j<img->height();j++)
     // 	       img->setPixel(i,j,matchPixel(img->pixel(i,j),map));
     int bytes = img->byteCount();
     Byte* data = img->bits();
     Frame frame(bytes);
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

     frames.push_back(frame);
     //delay.push_back(duration);
}
