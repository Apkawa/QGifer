/************************************************************************
** QGifer
** Copyright (C) 2013 Lukasz Chodyla <chodak166@op.pl>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
************************************************************************/

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
     QStringList sysEnv();
     QString findConvert();
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
