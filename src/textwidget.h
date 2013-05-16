#ifndef TEXTWIDGET_H
#define TEXTWIDGET_H

#include <QWidget>
#include "frameplayer.h"
#include "textobject.h"
#include "retranslatable.h"
#include "ui_textwidget.h"

class TextWidget : public QWidget, public Ui::TextWidget, public Retranslatable
{
     Q_OBJECT;
public:
     TextWidget(TextObject* to, FramePlayer* fp, QWidget* parent=0, Qt::WindowFlags f=0);
     TextWidget(FramePlayer* fp, QWidget* parent=0, Qt::WindowFlags f=0);
     virtual ~TextWidget();
     static QImage renderText(const QString& text,
			      const QFont& font,
			      const QColor& textColor,
			      const QColor& outlineColor,
			      int outlineWidth);
     static void renderText(TextObject* o);
     void setRange(int from, int to){fromBox->setValue(from);toBox->setValue(to);}
     void retranslate(){retranslateUi(this);setCaptions();}
private:
     void setCaptions();
     void paintEvent(QPaintEvent*);
     void setPlayer(FramePlayer* fp);
     void init();
     QImage renderText() const;
     FramePlayer* player;
     TextObject* textObject;
     bool editMode;
     private slots:
	  void insert();
	  void apply();
	  void setTColor();
	  void setOColor();
	  void validate();
	  void fromUpdate();
	  void toUpdate();

};

#endif
