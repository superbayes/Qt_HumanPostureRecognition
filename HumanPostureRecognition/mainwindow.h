#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "AI/myutils.h"
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QPainter>
#include <QThread>
#include <QTimer>
#include <omp.h>
#include "AI/nanodet_dancing.h"
#include "AI/nanodet_movenet.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public:
    NanoDet_Dancing* dancingDetor=nullptr;
    NanoDet_MoveNet* moveNetDetor=nullptr;
public:
    cv::VideoCapture cap;
    cv::Mat frame;
    QImage qimg;
    QPixmap qimg2pixmap;
    bool flag_closeVideoCapture = false;//是否关闭VideoCapture
    QTimer *m_pTimer;//定时获取video frame
private slots:
    void on_pb_open_clicked();

    void on_pb_close_clicked();

private:
    Ui::MainWindow *ui;
    //void paintEvent(QPaintEvent *event);
};
#endif // MAINWINDOW_H
