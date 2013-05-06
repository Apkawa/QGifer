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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QFileDialog>
#include "gifwidget.h"
#include "optimizerdialog.h"
#include "converterdialog.h"
#include "aboutdialog.h"
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
     Q_OBJECT;
public:
     MainWindow();
     virtual ~MainWindow();
     
private:
     bool openVideo(const QString& path);
     void connectMargins();
     void disconnectMargins();
     void resizeEvent(QResizeEvent*){if(correctionBox->isChecked())correctionChanged();}
     void closeEvent(QCloseEvent*e){saveSettings();QMainWindow::closeEvent(e);qApp->quit();}
     bool checkFFMPEG(){return !QProcess::execute("ffmpeg -version");}
     QImage finalFrame(long f);
     void correctRange();
     QSettings* set;
     QString vidfile;
     float whRatio;
     int ow;
     int oh;
     private slots:
	  void loadSettings();
	  void saveSettings();
	  void openVideo();
	  void extractGif();
	  void updatePalette();
	  void startFromCurrent() 
	  { startBox->setValue(player->getCurrentPos()); }
	  void stopFromCurrent() 
	  { stopBox->setValue(player->getCurrentPos()); }
	  void stopChanged(int v){multiSlider->setPosB(v,false);correctRange();}
	  void startChanged(int v);
	  void posAChanged(int);
	  void posBChanged(int);
	  void frameChanged(long);
	  void runOptimizer(){OptimizerDialog od(set); od.exec();}
	  void runConverter(){ConverterDialog cd(set); cd.exec();}
	  void marginBoxChanged(int s);
	  void gifSaved(const QString&);
	  void zoomChanged(int);
	  void ratioChanged(int);
	  void smoothChanged(int);
	  void applyMargins();
	  void marginsChanged();
	  void correctionChanged();
	  void resetCorrection();
	  void medianChanged(int m);
	  void lock(bool l);
	  void estimateOutputSize();
	  void outputWidthChanged(int);
	  void outputHeightChanged(int);
	  void whRatioChanged(int);
	  void varPaletteBoxChanged(int);
	  void about() {AboutDialog ad; ad.exec();}
	  void restoreDefault(){set->clear(); loadSettings();}
	  void openPalette();
	  void savePalette();
	  void insertObject();
	  void insertText();
	  void showProperties(WorkspaceObject*);
};

#endif
