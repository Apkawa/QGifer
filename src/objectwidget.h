#ifndef OBJECTWIDGET_H
#define OBJECTWIDGET_H

#include <QWidget>
#include "frameplayer.h"
#include "retranslatable.h"
#include "ui_objectwidget.h"

class ObjectWidget : public QWidget, public Ui::ObjectWidget, public Retranslatable
{
     Q_OBJECT;
public:
     ObjectWidget(FramePlayer* fp, QWidget* parent=0, Qt::WindowFlags f=0);
     ObjectWidget(WorkspaceObject* wo, FramePlayer* fp, QWidget* parent=0, Qt::WindowFlags f=0);
     virtual ~ObjectWidget();
     void setPlayer(FramePlayer* p);
     void setRange(int from, int to){fromBox->setValue(from);toBox->setValue(to);}
     void retranslate(){retranslateUi(this);}
private:
     void setWindowTitle()
     {QWidget::setWindowTitle(object?tr("Object properties"):tr("Insert object"));}
     WorkspaceObject* object;
     Workspace* wspace;
     FramePlayer* player;
     void init();
     private slots:
	  void insert();
	  void apply();
	  void imgSelect();
	  void fromUpdate();
	  void toUpdate();
	  void validate();
};

#endif
