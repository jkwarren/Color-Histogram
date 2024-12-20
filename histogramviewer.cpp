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
    QPixmap pixelBox = QPixmap(256, 256);
    pixelBox.fill();
    picLabel = new QLabel();
    picLabel->setPixmap(pixelBox);
    rightLayout->addWidget(picLabel, 4, Qt::AlignCenter);

    //slider for slices
    sliceSlider = new QSlider(Qt::Horizontal);
    sliceSlider->setMinimum(0);
    sliceSlider->setMaximum(255);
    sliceSlider->setSingleStep(1);
    rightLayout->addWidget(sliceSlider);

    //color opitions
    colorSelectLayout = new QHBoxLayout();
    rightLayout-> addLayout(colorSelectLayout);

    //main color for histogram
    colorCombobox = new QComboBox();
    QStringList colorOptions = QStringList() <<"red" <<"green" << "blue";
    colorCombobox->addItems(colorOptions);
    colorCombobox->setItemText(0,"red");
    connect(colorCombobox, &QComboBox::currentTextChanged, this, &HistogramViewer::ChangeComboboxColor);
    colorSelectLayout->addWidget(colorCombobox);

    //rgb sum option
    rgbSumCheckbox = new QCheckBox();
    connect(rgbSumCheckbox, &QCheckBox::clicked, this, &HistogramViewer::SelectRGBSliceView);
    colorSelectLayout->addWidget(rgbSumCheckbox);

    rgbSumLabel = new QLabel("check for r+g+b slice view");
    colorSelectLayout-> addWidget(rgbSumLabel);

    //scale combobox
    comboBoxLayout = new QHBoxLayout();
    rightLayout-> addLayout(comboBoxLayout, 2);

    comboBoxLabel = new QLabel("scale of pixel values");
    comboBoxLayout->addWidget(comboBoxLabel);

    //scale for pixel value on histogram
    scaleComboBox = new QComboBox();
    QStringList scaleOptions = QStringList() << "2" << "4" << "8" << "16" << "32" << "64" << "128" << "256";
    scaleComboBox-> addItems(scaleOptions);
    connect(scaleComboBox, &QComboBox::currentTextChanged, this, &HistogramViewer::ChangeScaleValue);
    comboBoxLayout->addWidget(scaleComboBox);

    //initialize pixmaps
    QPixmap blankPixmap = QPixmap(256, 256);
    blankPixmap.fill();
    pixSlices.resize(256, blankPixmap);

    //collect data
    histogramData.resize((1 << 24), 0);
    for (int y = 0; y < image.height(); ++y){
        //https://doc.qt.io/qt-6/qimage.html#scanLine
        //we use reinterpret cast bc the return
        //is a void pointer into memory so we need to specify the type
        QRgb *line = reinterpret_cast<QRgb*>(image.scanLine(y));

        for (int x=0; x < image.width(); ++x){
            QRgb &rgb = line[x];
            //qred does -> c &0xff0000 >> 16 where ff is the color we want in 0xAARRGGBB
            rgb = qRgba(qRed(rgb), qGreen(rgb), qBlue(rgb), 0);
            histogramData[rgb] +=1;

        }
    }


    LoadHistData(colorCombobox->currentText(), 2, rgbSumCheckbox->isChecked());
    connect(sliceSlider, &QSlider::sliderMoved, this, &HistogramViewer::ChangeSlice);
    picLabel->setPixmap(pixSlices[0]);


}

void HistogramViewer::LoadHistData(QString color, int scale, bool rgbSum){
    /*
     * Fills a cube of pixmaps representing all RRGGBB colors with ocurrences of a given pixel
     * color represented through it's number of ocurrences in the given image.
     *
     * Is able to slice by R+G+B value as well with rgbSum = true.
     */
    QApplication::processEvents();

    char pixmapColor = color == "red" ? 'r' : color == "green" ? 'g': 'b';
    QVector<QImage> imageSlices;
    QImage blankImage = QPixmap(256,256).toImage();
    blankImage.fill(Qt::white);
    imageSlices.resize(rgbSum? (255*3 + 1) : 256, blankImage);

    for (int iData = 0; iData < 1<<24; ++iData){
        if (iData % 1000000 == 0) QApplication::processEvents();
        uint primaryColorVal;
        uint xPixmap;
        uint yPixmap;
        switch(pixmapColor){
            case 'r':
                primaryColorVal = rgbSum? qRed(iData) + qGreen(iData)+ qBlue(iData): qRed(iData);
                xPixmap = qGreen(iData);
                yPixmap = qBlue(iData);
                break;
            case 'g':
                primaryColorVal = rgbSum? qRed(iData) + qGreen(iData)+ qBlue(iData):qGreen(iData);
                xPixmap = qRed(iData);
                yPixmap = qBlue(iData);
                break;
            case 'b':
                primaryColorVal = rgbSum? qRed(iData) + qGreen(iData)+ qBlue(iData):qBlue(iData);
                xPixmap = qRed(iData);
                yPixmap = qGreen(iData);
                break;
        }
        //making the color
         //50 ensures the color isn't any darker so it shows up on the pixmaps
        int color = 50 + scale * histogramData[iData];
        if (color > 255) color = 255;

        QColor pixelColor = QColor(color, color, color);
        imageSlices[primaryColorVal].setPixelColor(QPoint(xPixmap, yPixmap),
                                                   histogramData[iData] > 0 ? pixelColor : Qt::black);
    }

    if (rgbSum){
        for (int iSlice = 0; iSlice < 255*3 + 1; ++iSlice){
            pixSlices[iSlice] = QPixmap::fromImage(imageSlices[iSlice]);
        }
    }else{
        for (int iSlice = 0; iSlice < 256; ++iSlice){
            pixSlices[iSlice] = QPixmap::fromImage(imageSlices[iSlice]);
        }
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

    sliceSlider->setSliderPosition(0);
    LoadHistData(color, scaleComboBox->currentText().QString::toInt(), rgbSumCheckbox->isChecked());

    mainWindow->setEnabled(true);
    mainWindow->statusBar()->clearMessage();

}

void HistogramViewer::ChangeScaleValue(QString value){
    mainWindow->statusBar()->showMessage("loading in histogram for selected scale...");
    mainWindow->setEnabled(false);
    mainWindow->statusBar()->show();
    QApplication::processEvents();

    sliceSlider->setSliderPosition(0);
    LoadHistData(colorCombobox->currentText(), value.QString::toInt(), rgbSumCheckbox->isChecked());

    mainWindow->setEnabled(true);
    mainWindow->statusBar()->clearMessage();
}

void HistogramViewer::SelectRGBSliceView(){
    mainWindow->statusBar()->showMessage("loading in histogram for selected scale...");
    mainWindow->setEnabled(false);
    mainWindow->statusBar()->show();
    QApplication::processEvents();

    if (rgbSumCheckbox->isChecked()){
        sliceSlider->setMaximum(255*3 + 1);
        sliceSlider->setSliderPosition(0);

        QPixmap blankPixmap = QPixmap(256, 256);
        blankPixmap.fill();
        pixSlices.clear();
        pixSlices.resize(255*3 + 1, blankPixmap);
    }
    else{
        sliceSlider->setMaximum(256);
        sliceSlider->setSliderPosition(0);

        QPixmap blankPixmap = QPixmap(256, 256);
        blankPixmap.fill();
        pixSlices.clear();
        pixSlices.resize(256 , blankPixmap);
    }

    LoadHistData(colorCombobox->currentText(),scaleComboBox->currentText().QString::toInt(), rgbSumCheckbox->isChecked());

    mainWindow->setEnabled(true);
    mainWindow->statusBar()->clearMessage();
}
