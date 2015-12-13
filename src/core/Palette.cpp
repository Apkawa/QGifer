//
// Created by apkawa on 12/13/15.
//

#include <gifcreator.h>
#include <QtCore/qfile.h>
#include <QtCore/qtextstream.h>
#include <QtCore/qstringlist.h>
#include "Palette.h"


bool Palette::fromImage(const QImage &img, int palette_size, float mindiff) {
    clear();

    size = palette_size;
    QImage fimg = img.mirrored().convertToFormat(QImage::Format_RGB888);

    const int npix = fimg.width() * fimg.height();
    const int bytesPerLine = fimg.width() * 3;

    Frame r(npix), g(npix), b(npix);
    Frame output(npix);


    int cpix = 0;

    for (int rw = 0; rw < fimg.height(); rw++) {
        uchar *line = fimg.scanLine(rw);
        for (int i = 0; i + 2 < bytesPerLine; i += 3) {
            r[cpix] = line[i];
            g[cpix] = line[i + 1];
            b[cpix] = line[i + 2];
            cpix++;
        }
    }

    ColorMapObject *previous = palette;
    palette = MakeMapObject(size, NULL);

    if (!palette) {
        qDebug() << "NULL palette!";
        return false;
    }


    if (QuantizeBuffer(fimg.width(), fimg.height(), &size,
                       &(r[0]), &(g[0]), &(b[0]), &(output[0]),
                       palette->Colors) == GIF_ERROR) {
        qDebug() << "quentize failed!";
        palette = NULL;
        return false;
    }

    float df = difference(palette, previous);
    if (previous && df < mindiff) {
        FreeMapObject(palette);
        palette = previous;
    }
    else if (df >= mindiff) {
        FreeMapObject(previous);

    }

    return true;
}


float Palette::difference(ColorMapObject *a, ColorMapObject *b) {
    if (!a || !b) {
        return 0;
    }
    int same = 0;
    for (int c = 0; c < a->ColorCount; c++) {
        GifColorType &cc = a->Colors[c];
        for (int p = 0; p < b->ColorCount; p++) {
            GifColorType &pc = b->Colors[p];
            if (pc.Red == cc.Red && pc.Green == cc.Green && pc.Blue == cc.Blue)
                same++;
        }
    }
    return (float) same / (float) size;
}


QString Palette::toString() {
    QString str;
    for (int i = 0; i < palette->ColorCount; i++)
        str += QString::number(palette->Colors[i].Red) + ","
               + QString::number(palette->Colors[i].Green) + ","
               + QString::number(palette->Colors[i].Blue) + ";";
    return str;
}

bool Palette::fromString(const QString &str) {
    QStringList rgb = QString(str).split(";", QString::SkipEmptyParts);
    if (palette) {
        FreeMapObject(palette);
    }
    palette = MakeMapObject(rgb.size(), NULL);
    if (!palette)
        return false;
    size = rgb.size();
    qDebug() << "loaded palette size: " << size;
    for (int i = 0; i < size; i++) {
        QStringList c = rgb.at(i).split(",");
        if (c.size() != 3) {
            return false;
        }
        palette->Colors[i].Red = c.at(0).toInt();
        palette->Colors[i].Green = c.at(1).toInt();
        palette->Colors[i].Blue = c.at(2).toInt();
    }
    return true;
}

bool Palette::toFile(const QString &path) {
    if (!palette)
        return false;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;
    QTextStream out(&file);
    out << toString();
    file.close();
    return true;
}

bool Palette::fromFile(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    return fromString(file.readAll());
}

void Palette::setColor(unsigned int i, QColor color) {
    if(color.isValid())
    {
        palette->Colors[i].Red = color.red();
        palette->Colors[i].Green = color.green();
        palette->Colors[i].Blue = color.blue();
    }

}

QColor Palette::getColor(unsigned int i) {
    return QColor(
		     palette->Colors[i].Red,
		     palette->Colors[i].Green,
		     palette->Colors[i].Blue);

}
