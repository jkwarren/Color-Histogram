#include "mainwindow.h"
#include <QtWidgets>
#include "histogramviewer.h"
MainWindow *mainWindow;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mainWindow = this;

    QAction *openFileAct = new QAction("Open Image File");
    connect(openFileAct, &QAction::triggered, this, &MainWindow::openFileSlot);
    openFileAct -> setShortcut(Qt::CTRL | Qt::Key_O);

    QMenu *fileMenu = new QMenu("&File");
    fileMenu-> addAction(openFileAct);
    menuBar()-> addMenu(fileMenu);

    QSettings settings("JKW Systems", "Graphics1");
    lastDir = settings.value("lastDir", "").toString();

}

MainWindow::~MainWindow() {
    //quit program
    QSettings settings("JKW Systems", "Graphics1");
    settings.setValue("lastDir", lastDir);
}

void MainWindow::openFileSlot(){
    qDebug() << "file opened";
    mainWindow->statusBar()->showMessage("loading in histogram of colors...");
    mainWindow->setEnabled(false);
    mainWindow->statusBar()->show();
    QApplication::processEvents();

    QString fName = QFileDialog::getOpenFileName(this, "Select image file", lastDir, "Image files (*.png *.jpeg *.jpg *.bmp)");
    if (fName.isEmpty()) return;

    //gets last directory
    QString lastDir = QFileInfo(fName).absolutePath();

    //put image in QImage
    QImage image(fName);
    if (image.isNull()) return;

    HistogramViewer *histogramViewer = new HistogramViewer(image);
    setCentralWidget(histogramViewer);

    mainWindow->setEnabled(true);
    mainWindow->statusBar()->clearMessage();

}

//make a colorHistogram (Class)
//constructor: via Qimage

//a widget with an image viewer (stretchable)(possibly the one we made in class)
//to the right the 256x256 pixmap raster histogram (not stretchable)
// with a slider at the bottom that lets you selet what slice you are looking at (QSlider) 0...255
    //window bellow indicating the other color with a dropdown with red, green, blue QCombobox
//a given box, like if red, the amount of times red with a 1,0 occurs is a grey value (128,1,0) (128,0,0) (on the 128 slice of the cube)
//ex, if color is set to red, make a histogram with green vs blue and the graph changes as the red value changes
//the brightness is represented by the color (white if it occurs black if it doesnt) (or make it zero occurences with black, up to something is white, the contrast is a qslider)


