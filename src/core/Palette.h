//
// Created by apkawa on 12/13/15.
//

#ifndef QGIFER_PALETTE_H
#define QGIFER_PALETTE_H


#include <gif_lib.h>
#include <QtGui/qimage.h>
#include <QtGui/qcolor.h>

#include <QtCore/QDebug>

class Palette {
public:
    Palette() : size(256), palette(NULL) {

    }

    virtual ~Palette() {
        FreeMapObject(palette);
        palette = NULL;
    }

    ColorMapObject *map() { return palette; }

    ColorMapObject *mapCopy() { return MakeMapObject(palette->ColorCount, palette->Colors); }

    bool fromImage(const QImage &img, int palette_size, float mindiff = 2);

    bool toFile(const QString &path);

    bool fromFile(const QString &path);

    QString toString();

    bool fromString(const QString &str);


    void clear() {
        if (palette) FreeMapObject(palette);
        palette = NULL;
    }

    void setColor(unsigned int i, QColor color);

    QColor getColor(unsigned int i);

    int getSize() const {
        return size;
    }

    bool isEmpty() {
        return !palette || !palette->ColorCount;
    }

    ColorMapObject *getMap() {
        return palette;
    }

    ColorMapObject *getMapCopy() {
        return MakeMapObject(palette->ColorCount, palette->Colors);
    }


private:
    int size;
    ColorMapObject *palette;

    float difference(ColorMapObject *a, ColorMapObject *b);
};


#endif //QGIFER_PALETTE_H
