#include "workspace.h"

Workspace::Workspace(QWidget* parent, Qt::WindowFlags f):
     PreviewWidget(parent,f),useMr(false),frameIndex(-1),hoveredObject(NULL)
{
     canDrag = drag = mrNone;
}

Workspace::~Workspace()
{
     clear();
}

void Workspace::paintEvent(QPaintEvent* e)
{
     PreviewWidget::paintEvent(e);
     int x = (1-zoom)/2*width();
     int y = (1-zoom)/2*height();
     drawObjects(this,true,x,y);

     QPainter p(this);

     //marginesy
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


void Workspace::mouseMoveEvent(QMouseEvent* e)
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

     for(int i=0;i<objects.size();i++)
     {
	  WorkspaceObject* o = objects[i];
	  if(frameIndex >= o->getStart() && frameIndex <= o->getStop())
	  {
	       const QRect& r = o->previewRect();
	       //qDebug() << "checking cpos: " << cpos << ", over o rect: " << r;
	       if(o->currentMode() == WO::Moving)
	       {
		    o->setPosAt(frameIndex, 
				(float)cpos.x()/(float)image.width() - dx, 
				(float)cpos.y()/(float)image.height() - dy);
		    update();
	       }
	       else if(cpos.x() > r.x() && cpos.x() < r.x()+r.width() &&
		  cpos.y() > r.y() && cpos.y() < r.y()+r.height())
	       {
		    o->setMode(WO::Movable);
		    setCursor(Qt::SizeAllCursor);
		    hoveredObject = o;
		    update();
	       }
	       else if(o->currentMode() != WO::Normal)
	       {
		    o->setMode(WO::Normal);
		    setCursor(Qt::ArrowCursor);
		    update();
	       }
	       
	  }
     }
      

     //qDebug() << "mouse x,y: " << cpos;
}

void Workspace::mousePressEvent(QMouseEvent*)
{
     drag=canDrag;
     if(hoveredObject)
     {
	  hoveredObject->setMode(WO::Moving);
	  dx = (float)cpos.x()/(float)image.width() - hoveredObject->posAt(frameIndex).x;
	  dy = (float)cpos.y()/(float)image.height() - hoveredObject->posAt(frameIndex).y;
     }
}

void Workspace::mouseReleaseEvent(QMouseEvent*)
{
     emit clicked(normalizedX(),normalizedY());
     drag = mrNone;
     if(hoveredObject)
	  hoveredObject->setMode(WO::Movable);
}

void Workspace::updateMargins()
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



void Workspace::clear()
{
     PreviewWidget::clear();
     for(int i=0;i<objects.size();i++)
	  delete objects[i];
     objects.clear();
}

void Workspace::addObject(const QImage& img, int startFrame, int stopFrame)
{
     qDebug() << "adding object...";
     WorkspaceObject* w = new WorkspaceObject();
     w->setImage(img);
     w->setStartFrame(startFrame);
     w->setStopFrame(stopFrame);
     objects.append(w);
     qDebug() << "...done!";
}

void Workspace::drawObjects(QPaintDevice* pd, bool editMode, int x0, int y0)
{
     //qDebug() << "drawing objects...";
     //obiekty
     QPainter p(pd);
     if(frameIndex >= 0)
	  for(int i=0;i<objects.size();i++)
	  {
	       WorkspaceObject* o = objects[i];
	       if(frameIndex >= o->getStart() && frameIndex <= o->getStop())
	       {
		    QRect r;
		    if(pd == this)
			 r = o->updatePreviewRect(
			      frameIndex, image.size(), this->size(), origSize, zoom);
		    else
		    {
			 QSize pdsize(pd->width(),pd->height());
			 r = o->updatePreviewRect(
			      frameIndex, pdsize, pdsize, origSize, 1);
		    }

		    QImage simg = o->image()->scaled(r.width(), r.height(),
						     ratio ? Qt::KeepAspectRatio : 
						     Qt::IgnoreAspectRatio,
						     smooth || !editMode ? 
						     Qt::SmoothTransformation : 
						     Qt::FastTransformation);
		    p.drawImage(r.x(),r.y(),simg);
		    
		    if(!editMode)
			 continue;

		    //rysowanie ramki

	       }
	  }
     //qDebug() << "...done!";
}
