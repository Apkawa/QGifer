#ifndef WORKSPACE_H
#define WORKSPACE_H

#include "previewwidget.h"

class Workspace : public PreviewWidget
{
     Q_OBJECT;
public:
     Workspace(QWidget* parent=0, Qt::WindowFlags f=0);
     virtual ~Workspace();

private:

     private slots:

};

#endif
