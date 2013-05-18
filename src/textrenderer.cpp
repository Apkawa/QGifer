#include "textrenderer.h"

QImage TextRenderer::renderText(const QString& text,
			      const QFont& font,
			      const QColor& textColor,
			      const QColor& outlineColor,
			      int outlineWidth)
{
     if(text.isEmpty())
	  return QImage();
     QPen pen;
     pen.setColor(outlineColor);
     pen.setWidthF(outlineWidth);

     QFontMetrics fm(font);
     QRect r = fm.boundingRect(text);

     QPainterPath path;
     path.addText(r.width()*0.05, fm.height(), font, text);

     //QPixmap pix(QSize(path.boundingRect().width(), path.boundingRect().height()));
     QPixmap pix(QSize(r.width()*1.1f, r.height()*1.3f));
     pix.fill(Qt::transparent);
     QImage img = pix.toImage();

     QPainter p(&img);
     p.setRenderHints(QPainter::TextAntialiasing | QPainter::Antialiasing, true);

     p.setFont(font);
     p.setPen(pen);
     p.setBrush(textColor);
     p.drawPath(path);

     return img;
}

void TextRenderer::renderText(TextObject* o)
{
     o->setImage( renderText(o->getText(), o->getFont(), o->getTextColor(), 
			     o->getOutlineColor(), o->getOutlineWidth()) );
}
