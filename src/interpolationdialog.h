#ifndef INTERPOLATIONDIALOG_H
#define INTERPOLATIONDIALOG_H

#include <QDialog>
#include "workspaceobject.h"
#include "ui_interpolationdialog.h"

class InterpolationDialog : public QDialog, public Ui::InterpolationDialog
{
     Q_OBJECT;
public:
     enum Mode{Position,Size};

     InterpolationDialog(QWidget* parent, WorkspaceObject* object, 
			 InterpolationDialog::Mode = InterpolationDialog::Position);
     virtual ~InterpolationDialog();

private:
     WorkspaceObject* object;
     Mode mode;
     private slots:
	  void interpolate();
};

#endif
