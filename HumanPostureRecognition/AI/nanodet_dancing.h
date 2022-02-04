#pragma once
//
// Create by RangiLyu
// 2020 / 10 / 2
//

#ifndef NANODET_H
#define NANODET_H
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//#include "opencv2/highgui/highgui.hpp"
#include "net.h"

typedef struct HeadInfo_
{
    std::string cls_layer;
    std::string dis_layer;
    int stride;
} HeadInfo;

typedef struct CenterPrior_
{
    int x;
    int y;
    int stride;
} CenterPrior;

typedef struct BoxInfo {
    float x1;
    float y1;
    float x2;
    float y2;
    float score;
    int label;
} BoxInfo;

class NanoDet_Dancing {
public:
    NanoDet_Dancing(const char* param, const char* bin, bool useGPU=FALSE);

    ~NanoDet_Dancing();

    std::vector<BoxInfo> detect(cv::Mat &image, float score_threshold, float nms_threshold);
    void draw(cv::Mat& image, std::vector<BoxInfo> boxes);//在图像上绘制矩形
    std::vector<std::string> labels{ "body"};
    //std::vector<std::string> labels{ "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light",
    //                            "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow",
    //                            "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee",
    //                            "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard",
    //                            "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
    //                            "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch",
    //                            "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone",
    //                            "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear",
    //                            "hair drier", "toothbrush" };
private:
    void preprocess(cv::Mat& image, ncnn::Mat& in);
    void preprocess(
        cv::Mat& image, //in 原始图像
        float dst_size,  //in 输出图像的宽(宽高均为此值)
        ncnn::Mat& in,  //out,喂入推断网络的数据
        //float& wpad,      //out,x项填充的列数
        //float& hpad,      //out,y向填充的行数
        float& scale    //out,图像(宽或高)缩放的系数
    );

    void decode_infer(ncnn::Mat& feats, std::vector<CenterPrior>& center_priors, float threshold, std::vector<std::vector<BoxInfo>>& results, float width_ratio, float height_ratio);
    void decode_infer(ncnn::Mat& cls_pred, ncnn::Mat& dis_pred, int stride, float threshold, std::vector<std::vector<BoxInfo>>& results);

    BoxInfo disPred2Bbox(const float*& dfl_det, int label, float score, int x, int y, int stride, float width_ratio, float height_ratio);
    BoxInfo disPred2Bbox(const float*& dfl_det, int label, float score, int x, int y, int stride);

    static void nms(std::vector<BoxInfo>& result, float nms_threshold);

    ncnn::Net* Net;
    // modify these parameters to the same with your config if you want to use your own model
    float input_size = 416; // input height and width
    int num_class = 1; // number of classes. 80 for COCO
    int reg_max = 7; // `reg_max` set in the training config. Default: 7.
    std::vector<int> strides = { 8, 16, 32, 64 }; // strides of the multi-level feature.
    const float mean_vals[3] = { 103.53f, 116.28f, 123.675f };
    const float norm_vals[3] = { 0.017429f, 0.017507f, 0.017125f };

public:
    static NanoDet_Dancing* detector;
    static bool hasGPU;
};


#endif //NANODET_H
