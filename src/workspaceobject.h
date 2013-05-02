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

struct WOSize
{
WOSize(double iw = 0, double ih = 0):w(iw),h(ih){}
     double w;
     double h;
};

namespace WO{
     enum Mode{Normal=0, Movable=1, Moving=2, XLScalable=3, XLScaling=4,
	       XRScalable=5, XRScaling=6, YTScalable=7, YTScaling=8,
	       YBScalable=9, YBScaling=10, XYScalable=11, XYScaling=12};
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
     QSize originalSize() const {return img.size();}
     QSize sizeAt(int i) const {return QSize(img.size().width()*scale.at(i).w,
					     img.size().height()*scale.at(i).h);}
     const WOSize& scaleAt(int i) const {return scale.at(i-start);}
     void setScaleAt(int i, float xs, float ys);
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
     QList<WOSize> scale;
     private slots:

};

#endif
