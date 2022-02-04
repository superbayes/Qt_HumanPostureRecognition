#ifndef MYUTILS_H
#define MYUTILS_H

#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif
#ifndef INOUT
#define INOUT
#endif

#include <QObject>
#include <QCamera>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/imgcodecs.hpp>
//using namespace std;
//using namespace cv;
class MyUtils : public QObject
{
    Q_OBJECT
public:
    explicit MyUtils(QObject *parent = nullptr);
public:
    //Mat类型转换为QImage类型
    static void Mat2QImage(cv::Mat& cvImg,QImage& qimg);
    //根据输入的中心坐标绘制十字线
    static void drawCrossLine(cv::Mat& InOut,int offset_x=0,int offset_y=0);
    //根据输入的中心坐标绘制十字线,在加一个矩形框
    static void drawCrossLine2(cv::Mat& InOut,int offset_x=0,int offset_y=0,int rect_width = 100,int rect_height=100);
    //将一张图转换为一维向量
    static void matrix2vec(const cv::Mat& src,std::vector<float>& dst,int type=0);
    //枚举所有的设备ID
    static bool EnumerateCameras(std::vector<std::string> &camIdx);
    //虚像距,清晰度评价算法
    /**
      @brief:本函数用于评价单张图像的清晰度
      @param:src:输入的单通道的图像
      @param:rect:输入矩形,用于选择ROI区域,可以不适用
      @param:outValue1:清晰度指标的数值
      @param:thresh:Tenengrad算法的阈值,默认=5
      @return:
    */
    static void DefinitionFunc(
            IN cv::Mat& src,
            IN cv::Rect& rect,
            OUT float& outValue1,
            IN float Optimize=1.2
            );

    /*
    * 保持宽高比的缩放
    * src:input
    * dst:output
    * scale:output,缩放比例
    * dstSize:input,目标尺寸
    */
    static bool resize_keepRatio(
        cv::Mat& src,
        cv::Mat& dst,
        float& scale,
        int dstSize
    );
public:


signals:

};

#endif // MYUTILS_H
