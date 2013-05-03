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
     
     //pozycja
     while(d > pos.size())
     {
	  if(pos.size())
	       pos.append(pos.last());
	  else
	       pos.append(WOPos());
     }
     while(d < pos.size())
	  pos.removeLast();  

     //skala
     while(d > scale.size())
     {
	  if(scale.size())
	       scale.append(scale.last());
	  else
	       scale.append(WOSize(1,1));
     }
     while(d < scale.size())
	  scale.removeLast();
}

const QRect& WorkspaceObject::updatePreviewRect(
     int frame, const QSize& previewSize, 
     const QSize& workspaceSize, 
     const QSize& origFrameSize, double zoom)
{


     int x0 = (1-zoom)/2*workspaceSize.width();
     int y0 = (1-zoom)/2*workspaceSize.height();
     const WOPos& pos = posAt(frame);

     //qDebug() << "x0,y0: " << x0 <<","<<y0;

     int ox = x0+previewSize.width()*pos.x;
     int oy = y0+previewSize.height()*pos.y;
     //qDebug() << "ox,oy: " << ox << "," << oy;

     int swidth = sizeAt(frame).width()*
	  ((float)previewSize.width()/(float)origFrameSize.width());
     int sheight = sizeAt(frame).height()*
	  ((float)previewSize.height()/(float)origFrameSize.height());
     pwRect = QRect(ox,oy,swidth,sheight);
     return pwRect;
}

void WorkspaceObject::setScaleAt(int i, float xs, float ys)
{
     if(xs <= 0) xs = 1/(float)img.width();
     if(ys <= 0) ys = 1/(float)img.height();
     scale[i].w = xs; 
     scale[i].h = ys;
}

void WorkspaceObject::clonePosAt(int i, WO::Direction d)
{
     if(d == WO::Further)
	  for(int j=i+1;j<pos.size();j++)
	       pos[j] = pos[i];
     else
	  for(int j=i-1;j>=0;j--)
	       pos[j] = pos[i];
}

void WorkspaceObject::cloneScaleAt(int i, WO::Direction d)
{
     if(d == WO::Further)
	  for(int j=i+1;j<scale.size();j++)
	       scale[j] = scale[i];
     else
	  for(int j=i-1;j>=0;j--)
	       scale[j] = scale[i];
}
