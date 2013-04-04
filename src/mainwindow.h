#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QFileDialog>
#include "gifwidget.h"
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow
{
     Q_OBJECT;
public:
     MainWindow();
     virtual ~MainWindow();
     
private:
     bool openVideo(const QString& path);

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
};

#endif
