#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <QImage>
#include <QFont>
#include <QColor>
#include <QString>
#include <QPainter>
#include <QPen>
#include <QFontMetrics>
#include <QPixmap>
#include "textobject.h"

class TextRenderer {
public:
    static QImage renderText(const QString &text,
                             const QFont &font,
                             const QColor &textColor,
                             const QColor &outlineColor,
                             int outlineWidth);

    static void renderText(TextObject *o);
};

#endif
