#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_pTimer = new QTimer;
    m_pTimer->setInterval(100);  //定时30毫秒读取一帧数据
    dancingDetor = new NanoDet_Dancing(
                "./model/dancing_20220203-opt-fp16.param",
                "./model/dancing_20220203-opt-fp16.bin", TRUE);
    moveNetDetor = new NanoDet_MoveNet("./model/lightning", TRUE);

    connect(m_pTimer, &QTimer::timeout, this,[&](){
        if (!cap.isOpened())
        {
            if(m_pTimer->isActive())m_pTimer->stop();
            return;
        }
        cap >> frame;

        if (frame.empty())
        {
            if(m_pTimer->isActive())m_pTimer->stop();
            return;
        }

        //AI MODEL can insert here
        auto result = dancingDetor->detect(frame, 0.5, 0.5);
        //dancingDetor->draw(frame,result);
        if (result.empty())return;

        cv::Mat bodyPoseMat(frame.size(),CV_8UC3,cv::Scalar::all(0));
        for (BoxInfo rectBox: result)
        {
            if(rectBox.x1<0 || rectBox.x2<0||rectBox.y1<0||rectBox.y2<0){continue;}
            if(rectBox.x1>frame.cols ||rectBox.x2>frame.cols||rectBox.y1>frame.rows||rectBox.y2>frame.rows){continue;}
            cv::Rect rect(cv::Point(rectBox.x1, rectBox.y1), cv::Point(rectBox.x2, rectBox.y2));

            cv::Mat roiImg = frame(rect).clone();
            moveNetDetor->draw(roiImg, bodyPoseMat, rect.x, rect.y);
        }
        //=======================

        MyUtils::Mat2QImage(frame,qimg);
        QImage bodyPoseMat2qimg;
        MyUtils::Mat2QImage(bodyPoseMat,bodyPoseMat2qimg);
        this->qimg2pixmap = QPixmap::fromImage(qimg);
        this->ui->lab_cam1->setScaledContents(true);
        this->ui->lab_cam1->setPixmap(qimg2pixmap);//显示图像
        this->ui->lab_cam2->setScaledContents(true);
        this->ui->lab_cam2->setPixmap(QPixmap::fromImage(bodyPoseMat2qimg));//显示图像
    });


}

MainWindow::~MainWindow()
{
    delete ui;
    if(this->m_pTimer->isActive())m_pTimer->stop();
    this->cap.release();
    delete m_pTimer;
    if(dancingDetor!=nullptr)delete dancingDetor;
    if(moveNetDetor!=nullptr)delete  moveNetDetor;
}


void MainWindow::on_pb_open_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
                this, tr("open image file"),
                "./", tr("Image files(*.mp4 *.avi);;All files (*.*)"));

    if(fileName.isEmpty())
    {
        QMessageBox mesg;
        mesg.warning(this,"警告","打开video失败!");
        return;
    }else
    {
        qDebug()<<fileName;
        cap.open(fileName.toStdString());
        if(!cap.isOpened())return;
        m_pTimer->start();
    }

}

void MainWindow::on_pb_close_clicked()
{
    if(this->m_pTimer->isActive())m_pTimer->stop();
}

//void MainWindow::paintEvent(QPaintEvent *event)
//{
//    QPainter p;
//    p.begin(ui->lab_cam1);
//    p.drawPixmap(0,0,ui->lab_cam1->width(),ui->lab_cam1->height(),this->qimg2pixmap);
//    p.end();
//}
