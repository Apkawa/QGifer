#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "previewwidget.h"

class Workspace : public PreviewWidget
{
     Q_OBJECT;
public:
     Workspace(QWidget* parent=0, Qt::WindowFlags f=0);
     virtual ~Workspace();

     QMargins* margins(){return &mr;}
     void enableMargins(bool enable){useMr = enable;}

private:
     void paintEvent(QPaintEvent*);
     void mouseMoveEvent(QMouseEvent*);
     void mouseReleaseEvent(QMouseEvent*);
     void mousePressEvent(QMouseEvent*);
     void updateMargins(); //uruchamiane podczas przeciagania

     QMargins mr;
     bool useMr;
     QMargins pxMr; //marginesy po zoomowaniu
     
     enum Margin{mrLeft,mrTop,mrRight,mrBottom,mrNone};
     Margin canDrag;
     Margin drag;

     private slots:

};

#endif
