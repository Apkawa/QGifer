#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QFileDialog>
#include "gifwidget.h"
#include "optimizerdialog.h"
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

     QSettings* set;

     private slots:
	  void openVideo();
	  void extractGif();
	  void updatePalette();
	  void startFromCurrent() 
	  { startBox->setValue(player->getCurrentPos()); }
	  void stopFromCurrent() 
	  { stopBox->setValue(player->getCurrentPos()); }
	  void stopChanged(int v){multiSlider->setPosB(v,false);}
	  void startChanged(int v){multiSlider->setPosA(v,false);}
	  void posAChanged(int);
	  void posBChanged(int);
	  void frameChanged(long);
	  void runOptimizer(){OptimizerDialog od(set); od.exec();}
	  void marginBoxChanged(int s);
	  void gifSaved(const QString&);
	  void zoomChanged(int);
	  void ratioChanged(int);
	  void smoothChanged(int);
	  void applyMargins();
	  void marginsChanged();
};

#endif
