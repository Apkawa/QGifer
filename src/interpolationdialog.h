#ifndef INTERPOLATIONDIALOG_H
#define INTERPOLATIONDIALOG_H

#include <QDialog>
#include "workspaceobject.h"
#include "ui_interpolationdialog.h"

class InterpolationDialog : public QDialog, public Ui::InterpolationDialog
{
     Q_OBJECT;
public:
     enum Mode{Position=1,Size=2};

     InterpolationDialog(QWidget* parent, WorkspaceObject* object, 
			 int mode = (int)InterpolationDialog::Position);
     virtual ~InterpolationDialog();

private:
     WorkspaceObject* object;
     int mode;
     private slots:
	  void interpolate();
};

#endif
