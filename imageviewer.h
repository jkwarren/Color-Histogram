#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QGraphicsView>

class ImageViewer : public QGraphicsView //want it to look like s QGraphicsView but change it a bit (add features)
{
    Q_OBJECT

    QGraphicsScene scene;
    QImage image; //keeping a copy of an image so we can keep it to do things with
    QGraphicsPixmapItem *giPixmap;
    double scale;


public:
    ImageViewer(const QImage &_image);


signals:
    void mouseMoved(QPoint pos, QColor color);

protected:
    void keyPressEvent(QKeyEvent *evt) override;
    void mouseMoveEvent (QMouseEvent *evt) override;
};

#endif // IMAGEVIEWER_H

