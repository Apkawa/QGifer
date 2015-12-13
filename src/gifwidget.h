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

#ifndef GIFWIDGET_H
#define GIFWIDGET_H

#include <QDialog>
#include <QList>
#include <QSettings>
#include "qgifcreator.h"
#include "retranslatable.h"
#include "ui_gifwidget.h"

#include "utils/qgiferformatter.h"


class GifWidget : public QDialog, public Ui::GifWidget, public Retranslatable {
Q_OBJECT;
public:
    GifWidget(QSettings *s);

    virtual ~GifWidget();

    void addFrame(const QImage &f, ColorMapObject *map, bool dither);

    void setColorRes(int res) { gif->setColorRes(res); }

    void suggestName(const QString &name) { suggestedName = name; }

    void saveGif(const QString &gif);

    void retranslate() {
        QString t = windowTitle();
        retranslateUi(this);
        setWindowTitle(t);
    }

    unsigned long getEstimateSize();

    void setVisibleFPS(uint FPS) {
        visibleFPSBox->setValue(FPS);
    }

signals:

    void gifSaved(const QString &);

private:
    QGifCreator *gif;
    bool reversePlay;
    ColorMapObject *palette;
    QSettings *set;

    void createActions();

    QString suggestedName;
    QList<QImage> prevFrames;

    unsigned long frameByteSize = 0;

    int timerId;
    int currentFrame;
    int skipped;

    void timerEvent(QTimerEvent *);

public slots:

    void save();

    void play();

    void pause();

    void adjustWidgetSize() {
        adjustSize();
    }

    void updateEstimateSize() {
        estimateSize->setText(QGifer::utils::humanSizeFormat(this->getEstimateSize()));
    }

    void updateInterval();

};

#endif
