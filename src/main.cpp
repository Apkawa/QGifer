#include <QApplication>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
     QApplication app(argc,argv);
     app.setWindowIcon(QIcon(":/res/icon.png"));
     MainWindow mw;
     mw.show();
     return app.exec();
}
