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

#include "aboutdialog.h"

AboutDialog::AboutDialog()
{
     setupUi(this);
     logoLabel->setPixmap(
	  QPixmap(":/res/icon.png").scaled(
	       72,72,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
     connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
     infoLabel->setText("<span style=\"font-weight:bold;font-size:14pt;\">QGifer</span><br/>\
<span style=\"font-weight:bold;font-size:10pt;\">version "
             + QString(VERSION)
             + " </span><br/>\
        <span style=\"font-weight:normal;font-size:10pt;\">Qt version "
             + QT_VERSION_STR
             + "</span><br/>");
}

AboutDialog::~AboutDialog()
{

}
