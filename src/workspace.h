#ifndef WORKSPACE_H
#define WORKSPACE_H

#include <QList>
#include <QWheelEvent>
#include "previewwidget.h"
#include "workspaceobject.h"
#include "interpolationdialog.h"

#define WOSCALE_PX 3

class InterpolationDialog;

class Workspace : public PreviewWidget {
Q_OBJECT;
public:
    Workspace(QWidget *parent = 0, Qt::WindowFlags f = 0);

    virtual ~Workspace();

    QMargins *margins() { return &mr; }

    void enableMargins(bool enable) { useMr = enable; }

    void addObject(WorkspaceObject *wo) { objects.prepend(wo); }

    void addObject(const QString &imgPath, int startFrame, int stopFrame);

    void updateFrameIndex(int i) { frameIndex = i; }

    int currentFrameIndex() const { return frameIndex; }

    void clear();

    void drawObjects(QPaintDevice *pd);

    void drawObjects() { drawObjects(&image); }

    void drawSelection(QPaintDevice *pd);

    QList<WorkspaceObject *> *getObjectsList() { return &objects; }

    void enableFiltering(int h, int s, int v) {
        hue = h;
        sat = s;
        val = v;
    }

    void disableFiltering() { hue = sat = val = 0; }

    void enableAutoObjectDrawing(bool enable) { autoObjectDrawing = enable; }

    void setMarginsVisible(bool v) { displayMargins = v; }

signals:

    void propertiesRequested(WorkspaceObject *);

    void objectChanged();

    void objectHovered(WorkspaceObject *obj);

    void wheelRotated(int);

private:
    void paintEvent(QPaintEvent *);

    void mouseMoveEvent(QMouseEvent *);

    void mouseReleaseEvent(QMouseEvent *);

    void mousePressEvent(QMouseEvent *);

    void leaveEvent(QEvent *) {
        if (menuExecuted)return;
        hoverObject(-1, Qt::ArrowCursor);
    }

    void wheelEvent(QWheelEvent *e) { emit wheelRotated(e->delta()); }

    void updateMargins(); //uruchamiane podczas przeciagania
    void hoverObject(int i, const QCursor &c);

    void execObjectMenu(const QPoint &p);

    QList<WorkspaceObject *> objects;
    WorkspaceObject *hoveredObject;
    int hoIndex;
    bool displayMargins;
    int frameIndex;
    QMargins mr;
    bool useMr;
    float dx; //roznica miedzy kursorem a obiektem w poziomie
    float dy; //roznica miedzy kursorem a obiektem w pionie
    QSize coSize; //rozmiar kliknietego obiektu w momecie klikniecia
    QPoint clickPos; //pozycja klikniecia
    QMargins pxMr; //marginesy po zoomowaniu
    bool autoObjectDrawing; //czy rysować obiekty podczas rysowania workspace

    //korekcja
    int hue;
    int sat;
    int val;

    enum Margin {
        mrLeft, mrTop, mrRight, mrBottom, mrNone
    };
    Margin canDrag;
    Margin drag;
    bool lmbPressed;
    bool menuExecuted;
private slots:

};

#endif
