#ifndef INTERPOLATIONDIALOG_H
#define INTERPOLATIONDIALOG_H

#include <QDialog>
#include "workspace.h"
#include "retranslatable.h"
#include "ui_interpolationdialog.h"

class Workspace;

class InterpolationDialog : public QDialog, public Ui::InterpolationDialog, public Retranslatable {
Q_OBJECT;
public:
    enum Mode {
        Position = 1, Size = 2
    };

    InterpolationDialog(Workspace *workspace, WorkspaceObject *object,
                        int mode = (int) InterpolationDialog::Position);

    virtual ~InterpolationDialog();

    void retranslate() {
        retranslateUi(this);
        setWindowTitle();
    }

private:
    void setWindowTitle();

    WorkspaceObject *object;
    Workspace *workspace;
    int mode;
private slots:

    void interpolate();

    void toUpdate();

    void fromUpdate();
};

#endif
