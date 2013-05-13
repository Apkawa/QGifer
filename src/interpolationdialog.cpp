#include <QMessageBox>
#include "interpolationdialog.h"

InterpolationDialog::InterpolationDialog(QWidget* parent, WorkspaceObject* object,
     InterpolationDialog::Mode mode): QDialog(parent)
{
     setupUi(this);
     this->mode = mode;
     this->object = object;

     setWindowTitle(mode == Position ? tr("Interpolate object position") :
	  tr("Interpolate object size") );
     fromBox->setMaximum(object->getStop());
     fromBox->setMinimum(object->getStart());
     toBox->setMaximum(object->getStop());
     toBox->setMinimum(object->getStart()+2);
     toBox->setValue(toBox->maximum());

     connect(posButton, SIGNAL(clicked()), this, SLOT(interpolate()));
     connect(negButton, SIGNAL(clicked()), this, SLOT(close()));
}

InterpolationDialog::~InterpolationDialog()
{
     
}

void InterpolationDialog::interpolate()
{
     const int diff = toBox->value()-fromBox->value();
     if(diff < 2){
	  QMessageBox::warning(
			 this, tr("Warning"),
			 tr("The given range seems to be invalid, make sure that the stop frame is at least higher by two than the start frame!"));
	  return;
     }

     if(mode == Position)
     {
	  WOPos start = object->posAt(fromBox->value());
	  WOPos stop = object->posAt(toBox->value());
	  WOPos step( (stop.x-start.x)/diff, (stop.y-start.y)/diff );
	  for(int i=fromBox->value()+1, s = 1; i < toBox->value(); i++, s++)
	       object->setPosAt(i, start.x+step.x*s, start.y+step.y*s);
     }
     else
     {
	  WOSize start = object->scaleAt(fromBox->value());
	  WOSize stop = object->scaleAt(toBox->value());
	  WOSize step( (stop.w-start.w)/diff, (stop.h-start.h)/diff );
	  for(int i=fromBox->value()+1, s = 1; i < toBox->value(); i++, s++)
	       object->setScaleAt(i, start.w+step.w*s, start.h+step.h*s);
     }

     close();
}
