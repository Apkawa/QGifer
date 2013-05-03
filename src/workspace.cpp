#include "workspace.h"
#include <QMenu>
#include <QMessageBox>

Workspace::Workspace(QWidget* parent, Qt::WindowFlags f):
     PreviewWidget(parent,f),useMr(false),frameIndex(-1),hoveredObject(NULL),hoIndex(-1)
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
	       else if(o->currentMode() == WO::XRScaling) //skalowanie w prawo
	       {
		    float ncx = (float)cpos.x()/(float)image.width();
		    int newwidth = ncx*origSize.width()-o->posAt(frameIndex).x*origSize.width();
		    o->setScaleAt(frameIndex, (float)newwidth/(float)o->originalSize().width(), 
				  o->scaleAt(frameIndex).h);
		    update();
	       }
	       else if(o->currentMode() == WO::XLScaling) //skalowanie w lewo
	       {
		    float ncx = (float)cpos.x()/(float)image.width();
		    int newwidth = coSize.width()+(clickPos.x()-origSize.width()*ncx);
		    o->setScaleAt(frameIndex, (float)newwidth/(float)o->originalSize().width(),
			 o->scaleAt(frameIndex).h);
		    o->setPosAt(frameIndex, ncx, o->posAt(frameIndex).y);
		    update();
	       }
	       else if(o->currentMode() == WO::YBScaling) //skalowanie w dol
	       {
		    float ncy = (float)cpos.y()/(float)image.height();
		    int newheight = ncy*origSize.height()-o->posAt(frameIndex).y*origSize.height();
		    o->setScaleAt(frameIndex, o->scaleAt(frameIndex).w,
				  (float)newheight/(float)o->originalSize().height());
		    update();
	       }
	       else if(o->currentMode() == WO::YTScaling) //skalowanie w gore
	       {
		    float ncy = (float)cpos.y()/(float)image.height();
		    int newheight = coSize.height()+(clickPos.y()-origSize.height()*ncy);
		    o->setScaleAt(frameIndex, o->scaleAt(frameIndex).w, 
				  (float)newheight/(float)o->originalSize().height());
		    o->setPosAt(frameIndex, o->posAt(frameIndex).x, ncy);
		    update();
	       }
	       else if(o->currentMode() == WO::XYScaling) //skalowanie w dół i prawo z proporcjami
	       {
		    float ncx = (float)cpos.x()/(float)image.width();
		    int newwidth = ncx*origSize.width()-o->posAt(frameIndex).x*origSize.width();
		    float ncy = (float)cpos.y()/(float)image.height();
		    int newheight = ncy*origSize.height()-o->posAt(frameIndex).y*origSize.height();
		 
		    float ratio = (float)coSize.width()/(float)coSize.height();
		    qDebug() << "ratio: " << ratio;
		    //float ratio = (float)o->originalSize.width()/(float)o->originalSize.height();
		    // if(newwidth > newheight)
			 newheight = newwidth/ratio;
		    // else
		    // 	 newwidth = newheight*ratio;
		    o->setScaleAt(frameIndex, (float)newwidth/(float)o->originalSize().width(), 
				  o->scaleAt(frameIndex).h);
		    o->setScaleAt(frameIndex, o->scaleAt(frameIndex).w,
				  (float)newheight/(float)o->originalSize().height());
		    update();

	       }
	       else if (cpos.x() > r.x()+r.width()-WOSCALE_PX &&  //prawy dolny rog
			cpos.x() < r.x()+r.width()+WOSCALE_PX && 
			cpos.y() > r.y()+r.height()-WOSCALE_PX &&
			cpos.y() < r.y()+r.height()+WOSCALE_PX)
	       {
		    o->setMode(WO::XYScalable);
		    hoverObject(i, Qt::SizeFDiagCursor);
		    break;
	       }
	       else if(cpos.x() > r.x()+r.width()-WOSCALE_PX &&  //prawa krawedz
		       cpos.x() < r.x()+r.width()+WOSCALE_PX && 
		       cpos.y() > r.y() && cpos.y() < r.y()+r.height())
	       {
		    o->setMode(WO::XRScalable);
		    hoverObject(i, Qt::SizeHorCursor);
		    break;
	       }
	       else if(cpos.x() > r.x()-WOSCALE_PX && //lewa krawedz
		       cpos.x() < r.x()+WOSCALE_PX && 
		       cpos.y() > r.y() && cpos.y() < r.y()+r.height())
	       {
		    o->setMode(WO::XLScalable);
		    hoverObject(i, Qt::SizeHorCursor);
		    break;
	       }
	       else if(cpos.y() > r.y()+r.height()-WOSCALE_PX && //dolna krawedz
		       cpos.y() < r.y()+r.height()+WOSCALE_PX && 
		       cpos.x() > r.x() && cpos.x() < r.x()+r.width())
	       {
		    o->setMode(WO::YBScalable);
		    hoverObject(i, Qt::SizeVerCursor);
		    break;
	       }
	       else if(cpos.y() > r.y()-WOSCALE_PX && //gorna krawedz
		       cpos.y() < r.y()+WOSCALE_PX && 
		       cpos.x() > r.x() && cpos.x() < r.x()+r.width())
	       {
		    o->setMode(WO::YTScalable);
		    hoverObject(i, Qt::SizeVerCursor);
		    break;
	       }
	       else if(cpos.x() > r.x() && cpos.x() < r.x()+r.width() &&
		  cpos.y() > r.y() && cpos.y() < r.y()+r.height())
	       {
		    o->setMode(WO::Movable);
		    hoverObject(i, Qt::SizeAllCursor);
	       }
	       else if(o->currentMode() != WO::Normal)
	       {
		    o->setMode(WO::Normal);
		    hoverObject(-1, Qt::ArrowCursor);
		    break;
	       }
	       
	  }
     }
      

     //qDebug() << "mouse x,y: " << cpos;
}

void Workspace::mousePressEvent(QMouseEvent* e)
{
     drag=canDrag;
     if(hoveredObject && e->button() == Qt::LeftButton)
     {
	  dx = (float)cpos.x()/(float)image.width() - hoveredObject->posAt(frameIndex).x;
	  dy = (float)cpos.y()/(float)image.height() - hoveredObject->posAt(frameIndex).y;
	  hoveredObject->setMode( (WO::Mode)((int)hoveredObject->currentMode()+1));
	  coSize = hoveredObject->sizeAt(frameIndex);
	  clickPos = QPoint((float)cpos.x()/(float)image.width()*origSize.width(),
			    (float)cpos.y()/(float)image.height()*origSize.height());
     }
}

void Workspace::mouseReleaseEvent(QMouseEvent* e)
{
     emit clicked(normalizedX(),normalizedY());
     if(e->button() == Qt::RightButton && hoveredObject)
	  execObjectMenu(hoveredObject,e->globalPos());
     else
     {
	  drag = mrNone;
	  if(hoveredObject)
	       hoveredObject->setMode(WO::Movable);
     }
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
     objects.prepend(w);
     qDebug() << "...done!";
}

void Workspace::drawObjects(QPaintDevice* pd, bool editMode, int x0, int y0)
{
     //qDebug() << "drawing objects...";
     //obiekty
     QPainter p(pd);
     if(frameIndex >= 0)
	  for(int i=objects.size()-1;i>=0;i--)
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
						     //	     ratio ? Qt::KeepAspectRatio : 
						     Qt::IgnoreAspectRatio,
						     smooth || !editMode ? 
						     Qt::SmoothTransformation : 
						     Qt::FastTransformation);
		    p.drawImage(r.x(),r.y(),simg);
		    
		    if(!editMode || o != hoveredObject)
			 continue;

		    //rysowanie ramki
		    QPen pen = p.pen();
		    pen.setColor(Qt::yellow);
		    pen.setWidth(2);
		    pen.setStyle(Qt::DotLine);
		    p.setPen(pen);
		    p.drawRect(r);
		    p.fillRect(r.x()+r.width()-6, r.y()+r.height()-6, 7, 7, QColor(Qt::yellow));
	       }
	  }
     //qDebug() << "...done!";
}

void Workspace::execObjectMenu(WorkspaceObject*,const QPoint& p)
{
     	  QMenu* m = new QMenu(this);
	  QAction* btf = new QAction(tr("Bring to &front"),m);
	  m->addAction(btf);

	  QAction* stb = new QAction(tr("Send to &back"),m);
	  m->addAction(stb);

	  m->addSeparator();

	  QMenu* posM = new QMenu(tr("&Position"),m);
	  QAction* ffPos = new QAction(tr("Apply to the &further frames"),posM);
	  posM->addAction(ffPos);
	  QAction* efPos = new QAction(tr("Apply to the &earlier frames"),posM);
	  posM->addAction(efPos);
	  m->addMenu(posM);

	  QMenu* sizeM = new QMenu(tr("&Size"),m);
	  QAction* ffSize = new QAction(tr("Apply to the &further frames"),sizeM);
	  sizeM->addAction(ffSize);
	  QAction* efSize = new QAction(tr("Apply to the &earlier frames"),sizeM);
	  sizeM->addAction(efSize);
	  QAction* resetSize = new QAction(tr("&Restore original size"),sizeM);
	  sizeM->addAction(resetSize);
	  m->addMenu(sizeM);

	  m->addSeparator();

	  QAction* props = new QAction(tr("P&roperties..."),m);
	  m->addAction(props);

	  QAction* del = new QAction(tr("&Delete"),m);
	  m->addAction(del);

	  QAction* a = m->exec(p);
	  if(a == btf)
	       objects.prepend(objects.takeAt(hoIndex));
	  else if(a == stb)
	       objects.append(objects.takeAt(hoIndex));
	  else if(a == ffPos)
	       hoveredObject->clonePosAt(frameIndex, WO::Further);
	  else if(a == efPos)
	       hoveredObject->clonePosAt(frameIndex, WO::Earlier);
	  else if(a == ffSize)
	       hoveredObject->cloneScaleAt(frameIndex, WO::Further);
	  else if(a == efSize)
	       hoveredObject->cloneScaleAt(frameIndex, WO::Earlier);
	  else if(a == resetSize)
	       hoveredObject->setScaleAt(frameIndex, 1, 1);
	  else if(a == props)
	       ;
	  else if(a == del)
	       if(!QMessageBox::question(this, tr("Question"), 
					 tr("Do you really want to delete this object?"), 
					 tr("Yes"), tr("No")))
		    objects.removeAt(hoIndex);
	  delete m;
	  update();
}
