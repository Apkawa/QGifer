#ifndef GIFWIDGET_H
#define GIFWIDGET_H

#include <QWidget>
#include <QList>
#include <QSettings>
#include "qgifcreator.h"
#include "ui_gifwidget.h"

class GifWidget : public QWidget, public Ui::GifWidget
{
     Q_OBJECT;
public:
     GifWidget(QSettings* s);
     virtual ~GifWidget();
     void addFrame(const QImage& f);
     void setPalette(ColorMapObject* map){palette = map; gif.setPalette(map);}
     void suggestName(const QString& name){suggestedName = name;}
     void saveGif(const QString& gif);
signals:
     void gifSaved(const QString&);

private:
     QGifCreator gif;
     ColorMapObject* palette;
     QSettings* set;
     void createActions();
     QString suggestedName;
     QList<QImage> prevFrames;
     int timerId;
     int currentFrame;
     int skipped;
     void timerEvent(QTimerEvent*);
     public slots:
	  void save();
	  void play();
	  void pause();
	  void adjustWidgetSize(){adjustSize();}
};

#endif
