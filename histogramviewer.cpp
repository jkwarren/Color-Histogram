#include "histogramviewer.h"
#include <QtWidgets>
#include "imageviewer.h"
#include "mainwindow.h"


HistogramViewer::HistogramViewer(const QImage &_image)
    : image(_image)
{

    mainLayout = new QHBoxLayout();
    setLayout(mainLayout);

    //image to view
    imageViewer = new ImageViewer(_image);
    mainLayout-> addWidget(imageViewer,4, Qt::AlignLeft);

    //four widgets on the right
    rightLayout = new QVBoxLayout();
    mainLayout-> addLayout(rightLayout, 0);

    //histogram
    pixelBox = new QPixmap(256, 256);
    pixelBox->fill();
    picLabel = new QLabel();
    picLabel->setPixmap(*pixelBox);
    rightLayout->addWidget(picLabel, 4, Qt::AlignCenter);

    //slider for slices
    sliceSlider = new QSlider(Qt::Horizontal);
    sliceSlider->setMinimum(0);
    sliceSlider->setMaximum(255);
    sliceSlider->setSingleStep(1);
    rightLayout->addWidget(sliceSlider);

    //main color for histogram
    colorCombobox = new QComboBox();
    QStringList colorOptions = QStringList() <<"red" <<"green" << "blue";
    colorCombobox->addItems(colorOptions);
    colorCombobox->setItemText(0,"red");
    connect(colorCombobox, &QComboBox::currentTextChanged, this, &HistogramViewer::ChangeComboboxColor);
    rightLayout->addWidget(colorCombobox);


    spinBoxLayout = new QHBoxLayout();
    rightLayout-> addLayout(spinBoxLayout, 2);

    spinBoxLabel = new QLabel("scale of pixel values");
    spinBoxLayout->addWidget(spinBoxLabel);

    //scale for pixel value on histogram
    scaleSpinBox = new QSpinBox();
    scaleSpinBox->setRange(2, 256);
    scaleSpinBox->setSingleStep(2);
    connect(scaleSpinBox, &QSpinBox::valueChanged, this, &HistogramViewer::ChangeScaleValue);
    spinBoxLayout->addWidget(scaleSpinBox);


    QPixmap blankPixmap = QPixmap(256, 256);
    blankPixmap.fill();
    pixSlices.resize(256, blankPixmap);

    //collect data
    histogramData.resize((1 << 24), 0);
    totalPixels = 0;
    for (int y = 0; y < image.height(); ++y){
        //https://doc.qt.io/qt-6/qimage.html#scanLine
        //we use reinterpret cast bc the return
        //is a void pointer into memory so we need to specify the type
        QRgb *line = reinterpret_cast<QRgb*>(image.scanLine(y));

        for (int x=0; x < image.width(); ++x){
            QRgb &rgb = line[x];
            rgb = qRgba(qRed(rgb), qGreen(rgb), qBlue(rgb), 0);
            histogramData[rgb] +=1;
            totalPixels +=1;
        }
    }

    LoadHistData(colorCombobox->currentText(), scaleSpinBox->value());
    connect(sliceSlider, &QSlider::sliderMoved, this, &HistogramViewer::ChangeSlice);
    picLabel->setPixmap(pixSlices[0]);


}

void HistogramViewer::LoadHistData(QString color, int scale){
    QApplication::processEvents();

    char pixmapColor = color == "red" ? 'r' : color == "green" ? 'g': 'b';
    QVector<QImage> imageSlices;
    QImage blankImage = QPixmap(256,256).toImage();
    blankImage.fill(Qt::white);
    imageSlices.resize(256, blankImage);

    for (int iData = 0; iData < 1<<24; ++iData){
        if (iData % 1000000 == 0) QApplication::processEvents();
        uint primaryColorVal;
        uint xPixmap;
        uint yPixmap;
        switch(pixmapColor){
            case 'r':
                primaryColorVal = qRed(iData);
                xPixmap = qGreen(iData);
                yPixmap = qBlue(iData);
                break;
            case 'g':
                primaryColorVal = qGreen(iData);
                xPixmap = qRed(iData);
                yPixmap = qBlue(iData);
                break;
            case 'b':
                primaryColorVal = qBlue(iData);
                xPixmap = qRed(iData);
                yPixmap = qGreen(iData);
                break;
        }
        //making the color
         //160 ensures the color isn't any lighter so it shows up on the pixmaps
        int color = static_cast<int>(160 + scale * static_cast<float>((histogramData[iData]) / totalPixels));
        if (color > 160) color = 160;
        if (color < 0) color = 0;
        color = 255 - color; //have to reverse it so larger numbers are closer to zero (black)

        QColor pixelColor = QColor(color, color, color);
        imageSlices[primaryColorVal].setPixelColor(QPoint(xPixmap, yPixmap),
                                                   histogramData[iData] > 0 ? pixelColor : Qt::white);
    }

    for (int iSlice = 0; iSlice < 256; ++iSlice){
        pixSlices[iSlice] = QPixmap::fromImage(imageSlices[iSlice]);
    }

}

void HistogramViewer::ChangeSlice(int value){
    picLabel->setPixmap(pixSlices[value]);
}

void HistogramViewer::ChangeComboboxColor(QString color){
    mainWindow->statusBar()->showMessage("loading in histogram for selected color...");
    mainWindow->setEnabled(false);
    mainWindow->statusBar()->show();
    QApplication::processEvents();

    LoadHistData(color, scaleSpinBox->value());

    mainWindow->setEnabled(true);
    mainWindow->statusBar()->clearMessage();

}

void HistogramViewer::ChangeScaleValue(int value){
    mainWindow->statusBar()->showMessage("loading in histogram for selected scale...");
    mainWindow->setEnabled(false);
    mainWindow->statusBar()->show();
    QApplication::processEvents();

    LoadHistData(colorCombobox->currentText(), value);

    mainWindow->setEnabled(true);
    mainWindow->statusBar()->clearMessage();
}
