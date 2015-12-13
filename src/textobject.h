#ifndef TEXTOBJECT_H
#define TEXTOBJECT_H

#include "workspaceobject.h"
#include <QColor>
#include <QFont>

class TextObject : public WorkspaceObject {
Q_OBJECT;
public:
    TextObject() { };

    virtual ~TextObject() { };

    virtual QString getTypeName() const { return "TextObject"; }

    void setFont(const QFont &f) { font = f; }

    QFont getFont() const { return font; }

    void setTextColor(const QColor &tc) { textColor = tc; }

    QColor getTextColor() const { return textColor; }

    void setOutlineColor(const QColor &oc) { outlineColor = oc; }

    QColor getOutlineColor() const { return outlineColor; }

    void setOutlineWidth(int ow) { outlineWidth = ow; }

    int getOutlineWidth() const { return outlineWidth; }

    void setText(const QString &txt) { text = txt; }

    QString getText() const { return text; }

private:
    QFont font;
    QColor textColor;
    QColor outlineColor;
    int outlineWidth;
    QString text;
};

#endif
