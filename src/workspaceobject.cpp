#include "workspaceobject.h"
#include <QDebug>

WorkspaceObject::WorkspaceObject(QObject* parent):
     QObject(parent),start(0),mode(WO::Normal)
{

}

WorkspaceObject::~WorkspaceObject()
{

}

void WorkspaceObject::setPosAt(int i, double x, double y)
{
     pos[i-start].x = x;
     pos[i-start].y = y;
}

void WorkspaceObject::adjustPosList()
{
     int d = stop-start+1;
     if(d<=0)
	  return;
     while(d > pos.size())
     {
	  if(pos.size())
	       pos.append(pos.last());
	  else
	       pos.append(WOPos());
     }
     while(d < pos.size())
	  pos.removeLast();  
}

const QRect& WorkspaceObject::updatePreviewRect(
     int frame, const QSize& previewSize, 
     const QSize& workspaceSize, 
     const QSize& origFrameSize, double zoom)
{


     int x0 = (1-zoom)/2*workspaceSize.width();
     int y0 = (1-zoom)/2*workspaceSize.height();
     const WOPos& pos = posAt(frame);

     qDebug() << "x0,y0: " << x0 <<","<<y0;

     int ox = x0+previewSize.width()*pos.x;
     int oy = y0+previewSize.height()*pos.y;
     qDebug() << "ox,oy: " << ox << "," << oy;

     int swidth = size().width()*
	  ((float)previewSize.width()/(float)origFrameSize.width());
     int sheight = size().height()*
	  ((float)previewSize.height()/(float)origFrameSize.height());
     pwRect = QRect(ox,oy,swidth,sheight);
     return pwRect;
}
