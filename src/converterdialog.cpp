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

#include "converterdialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

ConverterDialog::ConverterDialog(QSettings *s) {
    setupUi(this);
    set = s;
    proc = new QProcess(this);
    msgLabel->hide();
    autonameBox->setChecked(set->value("converter_autoname", true).toBool());
    connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(srcButton, SIGNAL(clicked()), this, SLOT(setSrc()));
    connect(dstButton, SIGNAL(clicked()), this, SLOT(setDst()));
    connect(convertButton, SIGNAL(clicked()), this, SLOT(convert()));
    connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)),
            this, SLOT(finished(int, QProcess::ExitStatus)));
    connect(fromEdit, SIGNAL(timeChanged(QTime)), this, SLOT(timeChanged()));
    connect(toEdit, SIGNAL(timeChanged(QTime)), this, SLOT(timeChanged()));
}

ConverterDialog::~ConverterDialog() {
    set->setValue("converter_autoname", autonameBox->isChecked());
}

void ConverterDialog::convert() {
    QTime z(0, 0, 0);
    int start = z.secsTo(fromEdit->time());
    int length = fromEdit->time().secsTo(toEdit->time());

    qDebug() << "start: " << start;
    qDebug() << "length: " << length;

    if (length < 1) {
        QMessageBox::critical(this, tr("Error"),
                              tr("The length is invalid!"));
        return;
    }

    if (!QFile::exists(srcEdit->text())) {
        QMessageBox::critical(this, tr("Error"),
                              tr("The chosen source file does not exist!"));
        return;
    }

    QStringList args;
    args << "-i" << srcEdit->text() << "-y" << "-sameq" << "-vcodec" << "mjpeg" <<
    "-ss" << QString::number(start) << "-t" << QString::number(length) << dstEdit->text();
    proc->start("ffmpeg", args);
    msgLabel->setStyleSheet("color: green; font-size: 11pt");
    msgLabel->setText(tr("Converting, please wait..."));
    msgLabel->show();
    srcGroup->setEnabled(false);
    dstGroup->setEnabled(false);
    convertButton->setEnabled(false);
    qApp->processEvents();
}

void ConverterDialog::setSrc() {
    QString path = QFileDialog::getOpenFileName(
            this, tr("Open video file"),
            set->value("last_video_dir", qApp->applicationDirPath()).toString(),
            "video files (*.avi *.mp4 *.flv *.mpg);;all files(*)");
    if (!path.isEmpty()) {
        srcEdit->setText(path);
        if (autonameBox->isChecked())
            timeChanged();
        else
            dstEdit->setText(path.left(path.size() - 4) + "_mjpeg.avi");
    }
}

void ConverterDialog::setDst() {
    QString path = QFileDialog::getSaveFileName(
            this, tr("Save AVI file"),
            set->value("last_video_dir", qApp->applicationDirPath()).toString(),
            "AVI files (*.avi)");
    if (!path.isEmpty())
        dstEdit->setText(path);
}

void ConverterDialog::finished(int, QProcess::ExitStatus status) {
    if (status == QProcess::NormalExit && QFile::exists(dstEdit->text())) {
        msgLabel->setStyleSheet("color: green; font-size: 11pt");
        msgLabel->setText(tr("Done!"));
    }
    else if (status == QProcess::NormalExit) {
        msgLabel->setStyleSheet("color: red; font-size: 11pt");
        msgLabel->setText(tr("Can not write to destination file! (access denied?)"));
    }
    else {
        msgLabel->setStyleSheet("color: red; font-size: 11pt");
        msgLabel->setText(tr("Error: ") + QString(proc->readAllStandardError()));
    }
    msgLabel->show();
    srcGroup->setEnabled(true);
    dstGroup->setEnabled(true);
    convertButton->setEnabled(true);
}

void ConverterDialog::timeChanged() {
    if (!autonameBox->isChecked())
        return;
    if (!srcEdit->text().isEmpty() && QFile::exists(srcEdit->text())) {
        static const QString format = "HHmmss";
        dstEdit->setText(
                srcEdit->text().insert(
                        srcEdit->text().size() - 4,
                        "_" + fromEdit->time().toString(format) + toEdit->time().toString(format)));
    }
}
