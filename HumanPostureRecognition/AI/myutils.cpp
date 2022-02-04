#include "myutils.h"
#include <QCameraInfo>
#include <QImage>
#include <math.h>

MyUtils::MyUtils(QObject *parent) : QObject(parent)
{

}

//cvMat 转qimage
void MyUtils::Mat2QImage(cv::Mat& cvImg,QImage& qImg)
{
    if(cvImg.channels()==3)                             //3 channels color image
    {
        cv::cvtColor(cvImg,cvImg,cv::COLOR_BGR2RGB);
        qImg =QImage((const unsigned char*)(cvImg.data),
                    cvImg.cols, cvImg.rows,
                    cvImg.cols*cvImg.channels(),
                    QImage::Format_RGB888);
    }
    else if(cvImg.channels()==1)                    //grayscale image
    {
        qImg =QImage((const unsigned char*)(cvImg.data),
                    cvImg.cols,cvImg.rows,
                    cvImg.cols*cvImg.channels(),
                    QImage::Format_Indexed8);
    }
    else
    {
        qImg =QImage((const unsigned char*)(cvImg.data),
                    cvImg.cols,cvImg.rows,
                    cvImg.cols*cvImg.channels(),
                    QImage::Format_RGB888);
    }
}

void MyUtils::drawCrossLine(cv::Mat& InOut,int offset_x,int offset_y)
{
    int x = InOut.cols/2 + offset_x;
    int y = InOut.rows/2 + offset_y;

    cv::line(InOut,cv::Point(0,y),cv::Point(InOut.cols,y),cv::Scalar(0,0,255),1,16);//水平线
    cv::line(InOut,cv::Point(x,0),cv::Point(x,InOut.rows),cv::Scalar(0,0,255),1,16);//竖直线
}

void MyUtils::drawCrossLine2(cv::Mat &InOut, int offset_x, int offset_y, int rect_width, int rect_height)
{
    int x = InOut.cols/2 + offset_x;
    int y = InOut.rows/2 + offset_y;

    cv::line(InOut,cv::Point(0,y),cv::Point(InOut.cols,y),cv::Scalar(0,180,0),1,16);//水平线
    cv::line(InOut,cv::Point(x,0),cv::Point(x,InOut.rows),cv::Scalar(0,180,0),1,16);//竖直线

    cv::Rect rect(x-rect_width/2,y-rect_height/2,rect_width,rect_height);
    cv::rectangle(InOut,rect,cv::Scalar(0,255,0),1,cv::LINE_8);
}

void MyUtils::matrix2vec(const cv::Mat &src, std::vector<float> &dst, int type)
{

    if(src.empty())return;
    cv::Mat src2(src.size(),CV_8UC1,cv::Scalar::all(0));

    if(src.type()==CV_8UC3){cv::cvtColor(src,src2,cv::COLOR_BGR2GRAY);}
    else if(src.type()==CV_8UC1){src2 = src.clone();}
    else{return;}

    if(dst.size()!=src.cols)dst = std::vector<float>(src.cols,0.0f);

    uchar* p_src = (uchar*)src2.data;
    float* p_dst = dst.data();

    if(type==0)//遍历获取mat每一列的最大值
    {
        for(int col=0;col<src2.cols;col++)
        {
            int tmpMax = 0;
            for(int row=0;row<src2.rows;row++)
            {
                int adr = row*src2.cols + col;
                tmpMax = (p_src[adr]>tmpMax)?p_src[adr]:tmpMax;
            }
            p_dst[col] = tmpMax;
        }
    }
}

bool MyUtils::EnumerateCameras(std::vector<std::string> &camIdx)
{

    foreach(const QCameraInfo& CameraInfo,QCameraInfo::availableCameras() ){
        camIdx.push_back(CameraInfo.deviceName().toStdString());
    }
    return (camIdx.size()>0); // returns success
}

void MyUtils::DefinitionFunc(
        cv::Mat &src,
        cv::Rect &rect,
        float &outValue1,
        float Optimize)
{
    //Definition_Tenengrad(src,rect,outValue1,Optimize);
    //Definition_LSF(src,rect,outValue1,Optimize);
    return;
}

bool MyUtils::resize_keepRatio(cv::Mat &src, cv::Mat &dst, float &scale, int dstSize)
{
    float ratio_w = (float)dstSize / src.cols;
    float ratio_h = (float)dstSize / src.rows;
    cv::Mat tmpMat;
    if (ratio_w> ratio_h)
    {
        scale = ratio_h;
        cv::resize(src, tmpMat, cv::Size(0, 0), ratio_h, ratio_h, 1);
    }
    else
    {
        scale = ratio_w;
        cv::resize(src, tmpMat, cv::Size(0, 0), ratio_w, ratio_w, 1);
    }
    int w_pad = dstSize - tmpMat.cols;
    int h_pad = dstSize - tmpMat.rows;
    cv::copyMakeBorder(tmpMat, dst, 0, h_pad, 0, w_pad, cv::BORDER_CONSTANT, 0);
    return true;
}
