#ifndef QMULTISLIDER_H
#define QMULTISLIDER_H

#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QPoint>
#include <QMouseEvent>

class QMultiSlider : public QWidget
{
     Q_OBJECT;
public:
     QMultiSlider(QWidget* parent = NULL);
     virtual ~QMultiSlider();
     void setSkin(const QPixmap& h_bar, const QPixmap& mid_bar, 
		  const QPixmap& end_l, const QPixmap& end_r,
		  const QPixmap& a, const QPixmap& b)
     {pA = a; pB = b; bar = h_bar; midbar = mid_bar; endL = end_l; endR = end_r;}
     void setPosA(int p, bool emit_signal = true); 
     void setPosB(int p, bool emit_signal = true);
     int getPosA() const {return posA;}
     int getPosB() const {return posB;}
     void setMaximum(int m) {max = m;}
     void setMinimum(int m) {min = m;}
     int minimum() const {return min;}
     int maximum() const {return max;}
     int barWidth() const {return width()-pA.width();}

signals:
     void posAChanged(int);
     void posBChanged(int);

private:
     void paintEvent(QPaintEvent*);
     void mousePressEvent(QMouseEvent*);
     void mouseReleaseEvent(QMouseEvent*);
     void mouseMoveEvent(QMouseEvent*);
     //void updateAF(){af = (posA-min)/(max-min);}

     QPixmap bar;
     QPixmap midbar;
     QPixmap endL;
     QPixmap endR;
     QPixmap pA;
     QPixmap pB;
     QPoint pixPosA;
     QPoint pixPosB;
     int posA;
     int posB;
     int min;
     int max;
     int drag; //0 - nic, 1 - A, 2 - B
     float af;
     float bf;
     private slots:

};

#endif
