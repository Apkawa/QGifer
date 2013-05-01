#ifndef WORKSPACEOBJECT_H
#define WORKSPACEOBJECT_H

#include <QObject>
#include <QImage>
#include <QList>
#include <QRect>


struct WOPos
{
WOPos(double ix = 0, double iy = 0):x(ix),y(iy){}
     double x;
     double y;
};

namespace WO{
     enum Mode{Normal, Movable, Moving, XLeftScalable, XRightScalable, 
	       YTopScalable, YBottomScalable};
}

class WorkspaceObject : public QObject
{
     Q_OBJECT;
public:
     WorkspaceObject(QObject* parent = NULL);
     virtual ~WorkspaceObject();

     void setImage(const QImage& image) {img = image;}
     QImage* image() {return &img;}
     const WOPos& posAt(int i) const {return pos.at(i-start);}
     void setPosAt(int i, double x, double y);
     void setStartFrame(int s) {start = s;adjustPosList();}
     void setStopFrame(int s) {stop = s;adjustPosList();}
     int getStart() const {return start;}
     int getStop() const {return stop;}
     QSize size() const {return img.size();}
     void setPreviewRect(const QRect& r) {pwRect = r;}
     const QRect& updatePreviewRect(int frame, const QSize& previewSize, 
				    const QSize& workspaceSize, 
				    const QSize& origFrameSize, double zoom);
     const QRect& previewRect() const {return pwRect;}
     void setMode(WO::Mode m) {mode = m;}
     WO::Mode currentMode() const {return mode;}
private:
	  
     void adjustPosList();
     WO::Mode mode;
     QRect pwRect;
     QImage img;
     int start;
     int stop;
     QList<WOPos> pos;
     private slots:

};

#endif
