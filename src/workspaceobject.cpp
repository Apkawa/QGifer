#include "workspaceobject.h"
#include <QDebug>

WorkspaceObject::WorkspaceObject(QObject* parent):
     QObject(parent),start(0),stop(0),mode(WO::Normal)
{

}

WorkspaceObject::~WorkspaceObject()
{

}

void WorkspaceObject::setRange(int startFrame, int stopFrame)
{
     if(startFrame < 0)
	  startFrame = start;
     if(stopFrame < 0)
	  stopFrame = stop;
     const int newSize = stopFrame-startFrame+1;

     //pozycja 
     int startDiff = startFrame-start;
     while(startDiff > 0 && pos.size()){
	  pos.removeFirst();
	  startDiff--;
     }
     while(startDiff < 0){
	  pos.prepend( pos.size() ? pos.at(0) : WOPos() );
	  startDiff++;
     }

     int stopDiff = stopFrame-stop;
     while(stopDiff >= 0 && pos.size() < newSize){
	  pos.append( pos.size() ? pos.at(pos.size()-1) : WOPos() );
	  stopDiff--;
     }
     
     stopDiff = stopFrame-stop;
     while(stopDiff < 0 && pos.size()){
	  pos.removeLast();
	  stopDiff++;
     }

     //skala
     startDiff = startFrame-start;
     while(startDiff > 0 && scale.size()){
	  scale.removeFirst();
	  startDiff--;
     }
     while(startDiff < 0){
	  scale.prepend( scale.size() ? scale.at(0) : WOSize() );
	  startDiff++;
     }

     stopDiff = stopFrame-stop;
     while(stopDiff >= 0 && scale.size() < newSize){
	  scale.append( scale.size() ? scale.at(scale.size()-1) : WOSize() );
	  stopDiff--;
     }

     stopDiff = stopFrame-stop;
     while(stopDiff < 0 && scale.size()){
	  scale.removeLast();
	  stopDiff++;
     }
     
     start = startFrame;
     stop = stopFrame;

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
     i -= start;
     if(xs <= 0) xs = 1/(float)img.width();
     if(ys <= 0) ys = 1/(float)img.height();
     scale[i].w = xs; 
     scale[i].h = ys;
}

void WorkspaceObject::clonePosAt(int i, WO::Direction d)
{
     i = i-start;
     if(d == WO::Further)
	  for(int j=i+1;j<pos.size();j++)
	       pos[j] = pos[i];
     else
	  for(int j=i-1;j>=0;j--)
	       pos[j] = pos[i];
}

void WorkspaceObject::cloneScaleAt(int i, WO::Direction d)
{
     i = i-start;
     if(d == WO::Further)
	  for(int j=i+1;j<scale.size();j++)
	       scale[j] = scale[i];
     else
	  for(int j=i-1;j>=0;j--)
	       scale[j] = scale[i];
}
