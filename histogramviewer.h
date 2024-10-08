#ifndef HISTOGRAMVIEWER_H
#define HISTOGRAMVIEWER_H

#include <QtWidgets>
#include "mainwindow.h"
#include "imageviewer.h"

class HistogramViewer : public QWidget
{
    Q_OBJECT

    QImage image;
    QHBoxLayout *mainLayout;
    ImageViewer *imageViewer;

    QVBoxLayout *rightLayout;
    QPixmap *pixelBox;
    QLabel *picLabel;
    QSlider *sliceSlider;
    QComboBox *colorCombobox;

    QHBoxLayout *spinBoxLayout;
    QSpinBox *scaleSpinBox;
    QLabel *spinBoxLabel;

    QVector<int> histogramData;
    int totalPixels;
    QVector<QPixmap> pixSlices;


public:
    HistogramViewer(const QImage &_image);

private:
    void LoadHistData(QString color, int scale);

public slots:
    void ChangeSlice(int value);
    void ChangeComboboxColor(QString color);
    void ChangeScaleValue(int value);
};

#endif // HISTOGRAMVIEWER_H



//******************************can ask when we should bother with pointers
