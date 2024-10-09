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

    QHBoxLayout *colorSelectLayout;
    QComboBox *colorCombobox;
    QCheckBox *rgbSumCheckbox;
    QLabel *rgbSumLabel;

    QHBoxLayout *comboBoxLayout;
    QComboBox *scaleComboBox;
    QLabel *comboBoxLabel;

    QVector<int> histogramData;
    QVector<QPixmap> pixSlices;


public:
    HistogramViewer(const QImage &_image);

private:
    void LoadHistData(QString color, int scale, bool rgbSum);

public slots:
    void ChangeSlice(int value);
    void ChangeComboboxColor(QString color);
    void ChangeScaleValue(QString value);
    void SelectRGBSliceView();
};

#endif // HISTOGRAMVIEWER_H



//******************************can ask when we should bother with pointers
