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
};

#endif
