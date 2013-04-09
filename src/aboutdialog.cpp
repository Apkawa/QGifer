#include "aboutdialog.h"

AboutDialog::AboutDialog()
{
     setupUi(this);
     logoLabel->setPixmap(
	  QPixmap(":/res/icon.png").scaled(
	       72,72,Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
     connect(closeButton, SIGNAL(clicked()), this, SLOT(close()));
     infoLabel->setText("<span style=\"font-weight:bold;font-size:14pt;\">QGifer</span><br/>\
<span style=\"font-weight:bold;font-size:10pt;\">version "+
			QString::number(VERSION_MAJOR)+"."+
			QString::number(VERSION_MINOR)+"."+
			QString::number(VERSION_PATCH)+"</span><br/>\
<span style=\"font-weight:normal;font-size:10pt;\">Qt version "+QT_VERSION_STR+"</span><br/>");
}

AboutDialog::~AboutDialog()
{

}
