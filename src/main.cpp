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

#include <QApplication>
#include <QTextCodec>
#include <iostream>
#include "mainwindow.h"
#include "version.h"

void printUsage();

int main(int argc, char* argv[])
{
     QApplication* app = new QApplication(argc,argv);
     QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
     QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
     QString pro = "";
     if(argc == 2 && QFile::exists(QString(argv[1])))
	  pro = argv[1];
     else if(argc > 1){
	  std::cout << "======== QGifer v" << 
	       QString::number(VERSION_MAJOR).toStdString() << "." << 
	       QString::number(VERSION_MINOR).toStdString() << "." << 
	       QString::number(VERSION_PATCH).toStdString() << 
	       " beta ========" << std::endl << std::endl <<
	       "Usage: " << QString(argv[0]).toStdString() << 
	       " [project path]" << std::endl << std::endl;
	  delete app;
	  return 0;
     }

     app->setWindowIcon(QIcon(":/res/icon.png"));
     MainWindow* mw = new MainWindow();
     mw->show();
     app->processEvents();
     if(!pro.isEmpty())
	  mw->loadProject(pro);
     int r = app->exec();
     delete mw;
     delete app;
     return r;
}

void printUsage()
{
     
}
