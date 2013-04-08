#ifndef CONVERTERDIALOG_H
#define CONVERTERDIALOG_H

#include <QDialog>
#include <QSettings>
#include <QProcess>
#include "ui_converterdialog.h"

class ConverterDialog : public QDialog, public Ui::ConverterDialog
{
     Q_OBJECT;
public:
     ConverterDialog(QSettings* s);
     virtual ~ConverterDialog();
private:
     QProcess* proc;
     QSettings* set;
private slots:
     void convert();
     void setSrc();
     void setDst();
     void finished(int,QProcess::ExitStatus);
};

#endif
