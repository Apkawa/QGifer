#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QImage>
#include <QRect>
#include <QDebug>

#define MR_DRAG_PX 3

class PreviewWidget : public QWidget
{
     Q_OBJECT;
public:
     PreviewWidget(QWidget* parent=0, Qt::WindowFlags f=0);
     virtual ~PreviewWidget();
     void setImage(const QImage& img, const QSize& size = QSize(-1,-1));
     const QImage* getImage() const {return &image;}
     const QPoint* getCursorPos() const {return underMouse() ? &cpos : NULL;}
     const double normalizedX() const 
     {return image.isNull() ? -1 : (double)(cpos.x()-((1-zoom)/2*width()))/(double)image.width();}
     const double normalizedY() const 
     {return image.isNull() ? -1 : (double)(cpos.y()-((1-zoom)/2*height()))/(double)image.height();}
     void enableAntialiasing(bool enable) {smooth = enable;}
     void clear();
     void keepAspectRatio(bool keep){ratio = keep;}
     void setZoom(double z) {zoom = z;}
     QMargins* margins(){return &mr;}
     void enableMargins(bool enable){useMr = enable;}
private:
     void paintEvent(QPaintEvent*);
     void mouseMoveEvent(QMouseEvent*);
     void mouseReleaseEvent(QMouseEvent*);
     void mousePressEvent(QMouseEvent*);
     void updateMargins(); //uruchamiane podczas przeciagania
     double fit01(double v){if(v<0)return 0;else if(v>1) return 1; else return v;}
     QSize imsize;
     QSize origSize;
     QImage image;
     QPoint cpos;
     bool smooth;
     bool ratio;
     double zoom; //0 - 1
     QMargins mr;
     bool useMr;
     QMargins pxMr; //marginesy po zoomowaniu
     
     enum Margin{mrLeft,mrTop,mrRight,mrBottom,mrNone};
     Margin canDrag;
     Margin drag;

     private slots:
	  
signals:
     void clicked(double x, double y);
     void marginsChanged();
};

#endif


