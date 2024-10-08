#include "imageviewer.h"
#include<QtWidgets>


ImageViewer::ImageViewer(const QImage &_image): image(_image), scale(1.0){
    setScene(&scene);
    giPixmap = scene.addPixmap(QPixmap::fromImage(image));

    setBackgroundBrush(Qt::lightGray);
    setSceneRect(QRectF(QPointF(0,0), image.size()));
    setRenderHint(QPainter::Antialiasing, QPainter::SmoothPixmapTransform);
    setMouseTracking(true); //only if we want to track the mouse
    grabKeyboard();

}
void ImageViewer::keyPressEvent(QKeyEvent *evt) { //event is passed as a pointer

    if(evt -> key() == Qt::Key_Plus || evt -> key()==Qt::Key_Minus){
        scale *= (evt->key() == Qt:: Key_Plus) ? 2.0 : 0.5;
        giPixmap->setScale(scale);
        setSceneRect(QRectF(QPointF(0,0), image.size()*scale));

    } else{
        //to get the rest of the functionality we have to add it here
        QGraphicsView::keyPressEvent(evt); //does the default action as well
    }
}

void ImageViewer::mouseMoveEvent(QMouseEvent *evt){

    QPoint pos = (giPixmap->mapFromScene(mapToScene(evt->pos())) - QPointF(0.5, 0.5)).toPoint();
    if (image.valid(pos))
    {
        emit mouseMoved(pos, image.pixelColor(pos));

    }
}
