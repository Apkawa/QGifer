#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>
#include "version.h"
#include "ui_aboutdialog.h"

class AboutDialog : public QDialog, public Ui::AboutDialog
{
     Q_OBJECT;
public:
     AboutDialog();
     virtual ~AboutDialog();

private:

     private slots:

};

#endif
