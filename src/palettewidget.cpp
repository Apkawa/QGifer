#include "palettewidget.h"
#include <QPainter>
#include "qgifcreator.h"

PaletteWidget::PaletteWidget(QWidget* parent, Qt::WindowFlags f): 
     QWidget(parent,f), cols(10), palette(NULL), size(256), sqSize(0)
{

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

bool PaletteWidget::fromImage(const QImage& img, int palette_size)
{

     if(palette)
	  FreeMapObject(palette);

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
     
     palette = MakeMapObject(size, NULL);
     if (!palette) 
     {
	  qDebug() << "NULL palette!";
	  return false;
     }
     
     if (QuantizeBuffer(img.width(), img.height(), &size, 
			&(r[0]),&(g[0]),&(b[0]), &(output[0]), 
			palette->Colors) == GIF_ERROR) 
     {
	  qDebug() << "quentize failed!";
	  palette = NULL;
	  return false; 
     }

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
