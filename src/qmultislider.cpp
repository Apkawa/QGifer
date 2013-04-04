#include "qmultislider.h"
#include <QDebug>

QMultiSlider::QMultiSlider(QWidget* parent):QWidget(parent),min(0),max(100), drag(0)
{
     posA = minimum()+30;
     af = 0.3;
     posB = maximum()-30;
     bf = 0.7;
}

QMultiSlider::~QMultiSlider()
{
     
}

void QMultiSlider::paintEvent(QPaintEvent*)
{
     if(pA.isNull() || pB.isNull())
	  return;

     QPainter p(this);

     int half = pA.height()/2;
     int bp = half;
     int bary = half-bar.height()/2;
     p.drawPixmap(half,bary, endL);
     bp += endL.width();

     pixPosA = QPoint(barWidth()*af, 0);
     pixPosB = QPoint(barWidth()*bf, 0);

     while(bp<width()-pA.width()/2)
     {
	  p.drawPixmap(bp,bary, (bp>pixPosA.x() && bp < pixPosB.x()) ? midbar : bar);
	  bp += bar.width();
     }
     p.drawPixmap(width()-pA.width()/2,bary, endR);

     pixPosA = QPoint(barWidth()*af, 0);
     pixPosB = QPoint(barWidth()*bf, 0);
     p.drawPixmap(pixPosA, pA);
     p.drawPixmap(pixPosB, pB);
}


void QMultiSlider::mousePressEvent(QMouseEvent* e)
{
     if(e->x() > pixPosB.x() && e->x() < pixPosB.x()+pB.width() && e->y() < pB.height()){
	  drag = 2;
	  qDebug() << "dragging B";
     }
     else if(e->x() > pixPosA.x() && e->x() < pixPosA.x()+pA.width() && e->y() < pA.height()){
	  drag = 1;
	  qDebug() << "dragging A";
     }

}

void QMultiSlider::mouseReleaseEvent(QMouseEvent*)
{
     drag = 0;
}

void QMultiSlider::mouseMoveEvent(QMouseEvent* e)
{
     if(!drag)
	  return;

     float cf = e->x()/(float)width();
     float& f = (drag == 1) ? af : bf;
     int& pos = (drag == 1) ? posA : posB;    
     pos = (max-min)*cf+min;
     if(pos > max)
	  pos = max;
     else if(pos < min)
	  pos = min;

     if(drag == 1 && posA >= posB)
	  pos = posB;
     else if(drag == 2 && posB <= posA)
	  pos = posA;

     f = (min == max && max == 0) ? 0 : (float)(pos-min)/(float)(max-min);
     (drag == 1) ? (emit posAChanged(pos)) : (emit posBChanged(pos));
     repaint();
}

void QMultiSlider::setPosA(int p, bool emit_signal)
{
     posA = p;
     af = (min == max && max == 0) ? 0 : (float)(posA-min)/(float)(max-min);
     if(emit_signal)
	  emit posAChanged(p); 
     update();
}

void QMultiSlider::setPosB(int p, bool emit_signal)
{
     posB = p;
     bf = (min == max && max == 0) ? 0 : (float)(posB-min)/(float)(max-min);
     if(emit_signal)
	  emit posBChanged(p); 
     update();
}
