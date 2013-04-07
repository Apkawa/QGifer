#include "previewwidget.h"

PreviewWidget::PreviewWidget(QWidget* parent, Qt::WindowFlags f):
     QWidget(parent,f),smooth(false),imsize(-1,-1),ratio(true),zoom(1),useMr(false)
{
     setFixedSize(480,360);
     mr = QMargins(30,60,30,60);
     canDrag = drag = mrNone;
     show();
     setMouseTracking(true);
}

PreviewWidget::~PreviewWidget()
{
     qDebug() << "preview destroyed...";
}

void PreviewWidget::setImage(const QImage& img, const QSize& size)
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
	       imsize*zoom, ratio ? Qt::KeepAspectRatio : Qt::IgnoreAspectRatio, (
		    smooth ? Qt::SmoothTransformation : Qt::FastTransformation));
      	  //setFixedSize(imsize);
     // }
	  update();
	  // repaint();
}

void PreviewWidget::paintEvent(QPaintEvent*)
{
     if(image.isNull())
	  return;
     QPainter p(this);
     int x = (1-zoom)/2*width();
     int y = (1-zoom)/2*height();
     p.drawImage(x, y, image);

     QPen pen;
     pen.setColor(QColor(255,255,255));
     pen.setWidth(2);
     p.setPen(pen);
     if(useMr && !mr.isNull())
     {
	  
	  const QColor fc(0,0,0,200);
	  QPixmap top(image.width(),
		      (float)mr.top()/(float)origSize.height()*image.height());
	  top.fill(fc);
	  QPixmap bottom(image.width(),
	       (float)mr.bottom()/(float)origSize.height()*image.height());
	  bottom.fill(fc);

	  QPixmap left( (float)mr.left()/(float)origSize.width()*image.width()
		       ,image.height()-top.height()-bottom.height());
	  left.fill(fc);
	  QPixmap right((float)mr.right()/(float)origSize.width()*image.width()
		       ,image.height()-top.height()-bottom.height());
	  right.fill(fc);

	  pxMr = QMargins(x+left.width(), y+top.height(), 
			  x+image.width()-right.width(), y+image.height()-bottom.height());
	  p.drawPixmap(x,y,top);
	  p.drawPixmap(x,y+image.height()-bottom.height(),bottom);
	  p.drawPixmap(x,y+top.height(),left);
	  p.drawPixmap(x+image.width()-right.width(),y+top.height(),right);
     }
}


void PreviewWidget::mouseMoveEvent(QMouseEvent* e)
{
     cpos.setX(e->x());
     cpos.setY(e->y());

     if(drag != mrNone)
	  updateMargins();
     else if(useMr && !pxMr.isNull())
     {
	  
	  if(e->x() < pxMr.left() + MR_DRAG_PX && e->x() > pxMr.left() - MR_DRAG_PX)
	  {
	       setCursor(Qt::SizeHorCursor);
	       canDrag = mrLeft;
	  }
	  else if(e->x() < pxMr.right() + MR_DRAG_PX && e->x() > pxMr.right() - MR_DRAG_PX)
	  {
	       setCursor(Qt::SizeHorCursor);
	       canDrag = mrRight;
	  }
	  else if(e->y() < pxMr.top() + MR_DRAG_PX && e->y() > pxMr.top() - MR_DRAG_PX)
	  {
	       setCursor(Qt::SizeVerCursor);
	       canDrag = mrTop;
	  }
	  else if(e->y() < pxMr.bottom() + MR_DRAG_PX && e->y() > pxMr.bottom() - MR_DRAG_PX)
	  {
	       setCursor(Qt::SizeVerCursor);
	       canDrag = mrBottom;
	  }
	  else if(canDrag != mrNone)
	  {
	       setCursor(Qt::ArrowCursor);
	       canDrag = mrNone;
	  }
     }
     //qDebug() << "mouse x,y: " << cpos;
}

void PreviewWidget::mousePressEvent(QMouseEvent*)
{
     drag=canDrag;
}

void PreviewWidget::mouseReleaseEvent(QMouseEvent*)
{
     emit clicked(normalizedX(),normalizedY());
     drag = mrNone;
}

void PreviewWidget::clear()
{
     QPixmap p(QSize(1,1));
     p.fill(Qt::transparent);
     setImage(p.toImage());
}

void PreviewWidget::updateMargins()
{
     if(drag == mrNone)
	  return;
     if(drag == mrLeft){
	  int t = origSize.width()*fit01(normalizedX());
	  mr.setLeft(mr.right()+t <= origSize.width() ? t:
		     origSize.width()-mr.right());
     }
     else if(drag == mrRight){
	  int t = origSize.width()*(1-fit01(normalizedX()));
	  mr.setRight(mr.left()+t <= origSize.width() ? t:
		     origSize.width()-mr.left());
     }
     else if(drag == mrTop){
	  int t = origSize.height()*fit01(normalizedY());
	  mr.setTop(mr.bottom()+t <= origSize.height() ? t:
		     origSize.height()-mr.bottom());
     }
     else if(drag == mrBottom){
	  int t = origSize.height()*(1-fit01(normalizedY()));
	  mr.setBottom(mr.top()+t <= origSize.height() ? t:
		     origSize.height()-mr.top());
     }

     emit marginsChanged();
     update();
}

void PreviewWidget::applyBalance(QImage* img, int r, int g, int b)
{
     //*img = img->convertToFormat(QImage::Format_RGB888);
     const int step = img->format() == QImage::Format_RGB888 ? 3 : 4;

     for(int rw=0;rw<img->height();rw++)
     {
	  uchar* data = img->scanLine(rw);
	  for(int i=0;i<img->bytesPerLine();i+=step)
	  {
	       data[i] = data[i]+r > 255 ? 255 : data[i]+r < 0 ? 0 : data[i]+r;
	       data[i+1] = data[i+1]+g > 255 ? 255 : data[i+1]+g < 0 ? 0 : data[i+1]+g;
	       data[i+2] = data[i+2]+b > 255 ? 255 : data[i+2]+b < 0 ? 0 : data[i+2]+b;
	  }
     }


     // for(int i=0;i<img->byteCount();i+=step)
     // {
     // 	  data[i] = data[i]+r > 255 ? 255 : data[i]+r < 0 ? 0 : data[i]+r;
     // 	  data[i+1] = data[i+1]+g > 255 ? 255 : data[i+1]+g < 0 ? 0 : data[i+1]+g;
     // 	  data[i+2] = data[i+2]+b > 255 ? 255 : data[i+2]+b < 0 ? 0 : data[i+2]+b;
     // }
     // for(int i=0;i<9;i++)
     // 	  qDebug() << data[i];
}
