#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QGraphicsScene>

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QString lastDir;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void openFileSlot();
};
extern MainWindow *mainWindow;

#endif // MAINWINDOW_H
