#ifndef PALETTEWIDGET_H
#define PALETTEWIDGET_H

#include <QWidget>
#include <gif_lib.h>

class PaletteWidget : public QWidget
{
     Q_OBJECT;
public:
     PaletteWidget(QWidget* parent=0, Qt::WindowFlags f=0);
     virtual ~PaletteWidget();
     ColorMapObject* map() {return palette;}
     bool fromImage(const QImage& img, int palette_size);
     void setColumnCount(int cc){cols = cc;}
private:
     int size;
     int cols;
     int sqSize;
     ColorMapObject* palette;
     void paintEvent(QPaintEvent*);
     void resizeEvent(QResizeEvent*){sqSize = (width()-10)/cols;}
     private slots:

};

#endif
