/************************************************************************
** QGifer
** Copyright (C) 2013 Lukasz Chodyla <chodak166@op.pl>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program.  If not, see <http://www.gnu.org/licenses/>.
**
************************************************************************/

#include "palettewidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QColorDialog>

PaletteWidget::PaletteWidget(QWidget *parent, Qt::WindowFlags f) :
        QWidget(parent, f),
        cols(10),
        size(256),
        sqSize(0),
        hlIndex(-1) {
    palette_v2 = Palette();
    setMouseTracking(true);
}

PaletteWidget::~PaletteWidget() {
//    delete[] palette_v2;
}

void PaletteWidget::paintEvent(QPaintEvent *) {
    if (palette_v2.isEmpty()) {
        return;
    }
    QPainter p(this);

    int r = 0, c = 0;
    p.drawRect(0, 0, sqSize * cols + 1, sqSize * (getSize() / cols) + 1 + sqSize);
    for (int i = 0; i < getSize(); i++) {
        p.fillRect(1 + c * sqSize, 1 + r * sqSize, sqSize, sqSize, palette_v2.getColor(i));
        if (++c >= cols) {
            c = 0;
            r++;
        }
    }

    //powiekszenie podswietlonego
    if (hlIndex != -1) {
        QRect r(1 + (hlIndex % cols) * sqSize - 5, 1 + (hlIndex / cols) * sqSize - 5, sqSize + 10, sqSize + 10);
        p.fillRect(r, palette_v2.getColor(hlIndex));
        p.drawRect(r);
    }

}

void PaletteWidget::mouseReleaseEvent(QMouseEvent *e) {
    if (hlIndex == -1 || hlIndex >= getSize() || palette_v2.isEmpty())
        return;

    int i = hlIndex; //hlIndex zmieni sie w czasie wyswietlania dialogu
    QColor c = QColorDialog::getColor(palette_v2.getColor(i));
    palette_v2.setColor(i, c);

}

void PaletteWidget::mouseMoveEvent(QMouseEvent *e) {
    if (!sqSize || palette_v2.isEmpty()) {
        return;
    }
    int x = (e->x() - 1) / sqSize;
    int y = (e->y() - 1) / sqSize;

    if (x < 0 || y < 0 || x >= cols || y > getSize() / cols || cols * y + x >= getSize()) {
        hlIndex = -1;
        setCursor(Qt::ArrowCursor);
    }
    else {
        hlIndex = cols * y + x;
        setCursor(Qt::PointingHandCursor);
    }
    update();
}

bool PaletteWidget::fromImage(const QImage &img, int palette_size, float mindiff) {
    palette_v2.fromImage(img, palette_size, mindiff);
    reload();
    return true;
}


QString PaletteWidget::toString() {
    return palette_v2.toString();
}

bool PaletteWidget::fromString(const QString &str) {
    palette_v2.fromString(str);
    reload();
    return true;
}

bool PaletteWidget::toFile(const QString &path) {
    return palette_v2.toFile(path);
}

bool PaletteWidget::fromFile(const QString &path) {
    return palette_v2.fromFile(path);
}

