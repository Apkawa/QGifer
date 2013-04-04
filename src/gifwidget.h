#ifndef GIFWIDGET_H
#define GIFWIDGET_H

#include <QWidget>
#include <QList>
#include "qgifcreator.h"
#include "ui_gifwidget.h"

class GifWidget : public QWidget, public Ui::GifWidget
{
     Q_OBJECT;
public:
     GifWidget(QWidget* parent=0, Qt::WindowFlags f=0);
     virtual ~GifWidget();
     void addFrame(const QImage& f);
     void setPalette(ColorMapObject* map){palette = map; gif.setPalette(map);}
private:
     QGifCreator gif;
     ColorMapObject* palette;
     void createActions();
     QList<QImage> prevFrames;
     int timerId;
     int currentFrame;
     void timerEvent(QTimerEvent*);
     public slots:
	  void save();
	  void play();
	  void pause();
	  void adjustWidgetSize(){adjustSize();}
};

#endif
