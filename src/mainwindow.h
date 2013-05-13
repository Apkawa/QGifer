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
#include "textwidget.h"
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
     void closeEvent(QCloseEvent*e);
     bool checkFFMPEG(){return !QProcess::execute("ffmpeg -version");}
     QString relativeVideoPath();
     QString projectDir();
     QImage finalFrame(long f);
     void correctRange();
     QString projectToXml();
     bool projectFromXml(const QString& xstr);
     bool loadProject(const QString& file);
     bool isChanged() {return changed;}
     QSettings* set;
     QString vidfile;
     float whRatio;
     int ow;
     int oh;
     QString projectFile;
     bool changed;
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
	  void stopChanged();
	  void startChanged();
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
	  void newProject();
	  void openProject();
	  void saveProject();
	  void saveProject(const QString& file);
	  void saveProjectAs();
	  void setChanged(bool c = true);
	  void dockLevelChanged(bool top);

	  //view
	  void showOutputProp(){toolBox->setCurrentIndex(3);}
	  void showPreviewProp(){toolBox->setCurrentIndex(0);}
	  void showFilters(){toolBox->setCurrentIndex(2);}
	  void showMargins(){toolBox->setCurrentIndex(1);}
	  void toggleDock(){toolDock->setFloating(!toolDock->isFloating());}
};

#endif
