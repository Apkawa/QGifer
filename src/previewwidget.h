#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QImage>
#include <QRect>
#include <QDebug>


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
     {return image.isNull() ? -1 : (double)cpos.x()/(double)image.width();}
     const double normalizedY() const 
     {return image.isNull() ? -1 : (double)cpos.y()/(double)image.height();}
     void enableAntialiasing(bool enable) {smooth = enable;}
     void clear();
     void keepAspectRatio(bool keep){ratio = keep;}
     void setZoom(double z) {zoom = z;}
private:
     void paintEvent(QPaintEvent*);
     void mouseMoveEvent(QMouseEvent*);
     void mouseReleaseEvent(QMouseEvent*) {emit clicked(normalizedX(),normalizedY());}
     QSize imsize;
     QImage image;
     QPoint cpos;
     bool smooth;
     bool ratio;
     double zoom; //0 - 1

     private slots:
	  
signals:
     void clicked(double x, double y);

};

#endif


