// Tencent is pleased to support the open source community by making ncnn available.
//
// Copyright (C) 2021 THL A29 Limited, a Tencent company. All rights reserved.
//
// Licensed under the BSD 3-Clause License (the "License"); you may not use this file except
// in compliance with the License. You may obtain a copy of the License at
//
// https://opensource.org/licenses/BSD-3-Clause
//
// Unless required by applicable law or agreed to in writing, software distributed
// under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, either express or implied. See the License for the
// specific language governing permissions and limitations under the License.

#ifndef NANODET_MOVENET_H
#define NANODET_MOVENET_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <net.h>

struct keypoint
{
    float x;
    float y;
    float score;
};

class NanoDet_MoveNet
{
public:
    NanoDet_MoveNet();
    NanoDet_MoveNet(const char* modeltype, bool use_gpu = false);

    //int load(const char* modeltype, int target_size, const float* mean_vals, const float* norm_vals, bool use_gpu = false);

    int load(const char* modeltype, bool use_gpu = false);

    int detect(const cv::Mat& rgb);

    int draw(
        cv::Mat& rgbDet,//输入模型的图像
        cv::Mat& rgbDraw,//绘制模型输出结果的图像
        float offsetX = 0,
        float offsetY=0.0);

private:

    void detect_pose(cv::Mat &rgb, std::vector<keypoint> &points);
    ncnn::Net poseNet;
    int feature_size;
    float kpt_scale;
    int target_size = 192;//256
    float mean_vals[3]= { 127.5f, 127.5f,  127.5f };
    float norm_vals[3]= { 1 / 127.5f, 1 / 127.5f, 1 / 127.5f };
    std::vector<std::vector<float>> dist_y, dist_x;

    ncnn::UnlockedPoolAllocator blob_pool_allocator;
    ncnn::PoolAllocator workspace_pool_allocator;
};

#endif // NANODET_H
