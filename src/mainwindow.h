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
     void resizeEvent(QResizeEvent*){if(balanceBox->isChecked())balanceChanged();}
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
	  void balanceChanged();
	  void resetBalance();
	  void lock(bool l);
	  void estimateOutputSize();
	  void outputWidthChanged(int);
	  void outputHeightChanged(int);
	  void whRatioChanged(int);
	  void about() {AboutDialog ad; ad.exec();}
};

#endif
