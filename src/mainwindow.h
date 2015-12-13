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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QObject>
#include <QFileDialog>
#include <QTranslator>
#include <QToolButton>
#include "gifwidget.h"
#include "optimizerdialog.h"
#include "converterdialog.h"
#include "aboutdialog.h"
#include "textwidget.h"
#include "retranslatable.h"
#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, private Ui::MainWindow, public Retranslatable {
Q_OBJECT;
public:
    MainWindow();

    virtual ~MainWindow();

    void retranslate() {
        QString t = windowTitle();
        retranslateUi(this);
        renderDefaultText();
        setWindowTitle(t);
    }

    bool loadProject(const QString &file);

private:

    bool openVideo(const QString &path);

    void connectMargins();

    void disconnectMargins();

    void resizeEvent(QResizeEvent *) {
        if (correctionBox->isChecked())correctionChanged();
    }

    void closeEvent(QCloseEvent *e);

    bool checkFFMPEG() { return !QProcess::execute("ffmpeg -version"); }

    QString projectRelativePath(const QString &path);

    QString projectDir();

    QImage finalFrame(long f);

    void correctRange();

    QString projectToXml();

    bool projectFromXml(const QString &xstr);

    bool isChanged() { return changed; }

    QString dataDir();

    void loadLanguages(); //uzupelnia hash 'langs' i tworzy menu wyboru jezyka
    void loadLanguage(const QString &basename, const QString &qmpath);

    void renderDefaultText();

    QSettings *set;
    QString vidfile;
    float whRatio;
    int ow;
    int oh;
    QString projectFile;
    bool locked;
    bool changed;
    QHash<QAction *, QString> langs;
    QTranslator *translator;

private slots:

    void loadSettings();

    void saveSettings();

    void openVideo();

    void extractGif();

    void updatePalette();

    void startFromCurrent() {
        startBox->setValue(player->getCurrentPos());
        startChanged();
    }

    void stopFromCurrent() {
        stopBox->setValue(player->getCurrentPos());
        stopChanged();
    }

    void jumpToStart() {
        player->seek(startBox->value());
    };

    void jumpToStop() {
        player->seek(stopBox->value());
    };

    void stopChanged();

    void startChanged();

    void posAChanged(int);

    void posBChanged(int);

    void frameChanged(long);

    void runOptimizer() {
        OptimizerDialog od(set);
        od.exec();
    }

    void runConverter() {
        ConverterDialog cd(set);
        cd.exec();
    }

    void marginBoxChanged(int s);

    void gifSaved(const QString &);

    void zoomChanged(int);

    void ratioChanged(int);

    void smoothChanged(int);

    void applyMargins();

    void marginsChanged();

    void correctionChanged();

    void resetCorrection();

    void medianChanged(int m);

    void lock(bool l);

    void estimateOutputSize();

    void outputWidthChanged(int);

    void outputHeightChanged(int);

    void whRatioChanged(bool);

    void varPaletteBoxChanged(int);

    void autoPaletteBoxChanged(int);

    void about() {
        AboutDialog ad;
        ad.exec();
    }

    void restoreDefault() {
        set->clear();
        loadSettings();
    }

    void openPalette();

    void savePalette();

    void insertObject();

    void insertText();

    void showProperties(WorkspaceObject *);

    void newProject();

    void openProject();

    void saveProject();

    void saveProject(const QString &file);

    void saveProjectAs();

    void setChanged(bool c = true);

    void dockLevelChanged(bool top);

    void languageChanged(QAction *);

    void workspaceWheelRotated(int);

    void drawBkgToggled(bool c) {
        player->getWorkspace()->enableBackground(c);
        player->getWorkspace()->update();
    }

    //view
    void showOutputProp() { toolBox->setCurrentIndex(3); }

    void showPreviewProp() { toolBox->setCurrentIndex(0); }

    void showFilters() { toolBox->setCurrentIndex(2); }

    void showMargins() { toolBox->setCurrentIndex(1); }

    void toggleDock() { toolDock->setFloating(!toolDock->isFloating()); }
};

#endif
