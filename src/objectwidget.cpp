#include <QSettings>
#include <QFileDialog>
#include "objectwidget.h"

ObjectWidget::ObjectWidget(WorkspaceObject* wo, FramePlayer* fp, QWidget* parent, Qt::WindowFlags f): 
     object(wo), wspace(NULL)
{
     init();
     setPlayer(fp);
     setWindowTitle(tr("Insert object"));
     posButton->setText(tr("Apply"));
     negButton->setText(tr("Close"));
     fromBox->setValue(object->getStart());
     toBox->setValue(object->getStop());
     imgEdit->setText(object->getImagePath());
     validate();
     connect(posButton, SIGNAL(clicked()), this, SLOT(apply()));   
}

ObjectWidget::ObjectWidget(Workspace* ws, FramePlayer* fp, QWidget* parent, Qt::WindowFlags f): 
     wspace(ws), object(NULL)
{
     init();
     setPlayer(fp);
     setWindowTitle(tr("Object properties"));
     posButton->setText(tr("Insert"));
     negButton->setText(tr("Cancel"));
     connect(posButton, SIGNAL(clicked()), this, SLOT(insert()));
}

ObjectWidget::~ObjectWidget()
{

}

void ObjectWidget::init()
{
     setupUi(this);
     validate();
     connect(negButton, SIGNAL(clicked()), this, SLOT(close()));
     connect(imgButton, SIGNAL(clicked()), this, SLOT(imgSelect()));
     connect(curFromButton, SIGNAL(clicked()), this, SLOT(fromUpdate()));
     connect(curToButton, SIGNAL(clicked()), this, SLOT(toUpdate()));
}

void ObjectWidget::insert()
{
     wspace->addObject(imgEdit->text(), fromBox->value(), toBox->value());
     close();
}

void ObjectWidget::apply()
{
     if(!object)
	  return;
     object->setImage(imgEdit->text());
     object->setStartFrame(fromBox->value());
     object->setStopFrame(toBox->value());
     close();
}

void ObjectWidget::validate()
{
     posButton->setEnabled(!imgEdit->text().isEmpty() && QFile::exists(imgEdit->text())
			   && fromBox->value() <= toBox->value() && player && player->countFrames());
     
}

void ObjectWidget::imgSelect()
{
     QSettings set("QGifer","QGifer");
     QString path = QFileDialog::getOpenFileName(
	  this, tr("Open image"), 
	  set.value("last_objects_dir","").toString(),
	  "Image files (*.png *.bmp *.jpg *.jpeg);;All files (*)");
     if(!path.isEmpty())
     {
	  imgEdit->setText(path);
	  set.setValue("last_objects_dir", path);
     }
     validate();
}

void ObjectWidget::fromUpdate()
{
     if(player)
	  fromBox->setValue(player->getCurrentPos());
     validate();
}

void ObjectWidget::toUpdate()
{
     if(player)
	  toBox->setValue(player->getCurrentPos());
     validate();
}

void ObjectWidget::setPlayer(FramePlayer* p)
{
     player = p;
     fromBox->setMaximum(player->countFrames());
     toBox->setMaximum(player->countFrames());
     validate();
}

