#ifndef QGIFCREATOR_H
#define QGIFCREATOR_H

#include <QObject>
#include <QImage>
#include <QList>
#include <QDebug>
#include "gifcreator.h"

class QGifCreator : public QObject, public GifCreator
{
     Q_OBJECT;
public:
     QGifCreator();
     virtual ~QGifCreator();
     void setDuration(float d){duration = d;}
     Byte* imageData(const QImage& img);
     void prepareFrame(QImage* img, ColorMapObject* map);
private:
     QList<QImage*> cache;
     float duration;
     private slots:

};

#endif
