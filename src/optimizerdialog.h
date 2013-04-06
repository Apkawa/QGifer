#ifndef OPTIMIZERDIALOG_H
#define OPTIMIZERDIALOG_H

#include <QDialog>
#include <QProcess>
#include <QSettings>
#include "ui_optimizerdialog.h"

class OptimizerDialog : public QDialog, public Ui::OptimizerDialog
{
     Q_OBJECT;
public:
     OptimizerDialog(QSettings* s);
     virtual ~OptimizerDialog();

private:
     bool convertAvailable();
     void checkIM();
     QProcess* proc;
     QSettings* set;
     private slots:
	  void optimize();
	  void setIMDir();
	  void setSrc();
	  void setDst();
	  void finished(int,QProcess::ExitStatus);
	  void showStateChanged(int);
};

#endif
