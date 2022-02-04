#include "nanodet_dancing.h"
bool NanoDet_Dancing::hasGPU = true;
NanoDet_Dancing* NanoDet_Dancing::detector = nullptr;
inline float fast_exp(float x)
{
    union {
        uint32_t i;
        float f;
    } v{};
    v.i = (1 << 23) * (1.4426950409 * x + 126.93490512f);
    return v.f;
}
inline float sigmoid(float x)
{
    return 1.0f / (1.0f + fast_exp(-x));
}

template<typename _Tp>
int activation_function_softmax(const _Tp* src, _Tp* dst, int length)
{
    const _Tp alpha = *std::max_element(src, src + length);
    _Tp denominator{ 0 };

    for (int i = 0; i < length; ++i) {
        dst[i] = fast_exp(src[i] - alpha);
        denominator += dst[i];
    }

    for (int i = 0; i < length; ++i) {
        dst[i] /= denominator;
    }

    return 0;
}

static void generate_grid_center_priors(const int input_height, const int input_width, std::vector<int>& strides, std::vector<CenterPrior>& center_priors)
{
    for (int i = 0; i < (int)strides.size(); i++)
    {
        int stride = strides[i];
        int feat_w = ceil((float)input_width / stride);
        int feat_h = ceil((float)input_height / stride);
        for (int y = 0; y < feat_h; y++)
        {
            for (int x = 0; x < feat_w; x++)
            {
                CenterPrior ct;
                ct.x = x;
                ct.y = y;
                ct.stride = stride;
                center_priors.push_back(ct);
            }
        }
    }
}

NanoDet_Dancing::NanoDet_Dancing(const char* param, const char* bin, bool useGPU)
{
    this->Net = new ncnn::Net();
    hasGPU = ncnn::get_gpu_count() > 0;
    this->Net->opt.use_vulkan_compute = useGPU; //hasGPU && useGPU;  // gpu
    this->Net->opt.use_fp16_arithmetic = true;
    this->Net->opt.use_fp16_packed = true;
    this->Net->opt.use_fp16_storage = true;
    this->Net->load_param(param);
    this->Net->load_model(bin);
}

NanoDet_Dancing::~NanoDet_Dancing()
{
    delete this->Net;
}

std::vector<BoxInfo> NanoDet_Dancing::detect(cv::Mat& image, float score_threshold, float nms_threshold)
{
  
    ncnn::Mat input;
    float /*halfWpad, halfHpad,*/ scale;
    preprocess(image, this->input_size, input, /*halfWpad, halfHpad,*/ scale);
    auto ex = this->Net->create_extractor();
    ex.set_light_mode(true);
    ex.set_num_threads(4);
    //hasGPU = ncnn::get_gpu_count() > 0;
    //ex.set_vulkan_compute(hasGPU);
    ex.input("data", input);
    std::vector<std::vector<BoxInfo>> results;
    results.resize(this->num_class);
    ncnn::Mat out;
    ex.extract("output", out);

    // generate center priors in format of (x, y, stride)
    std::vector<CenterPrior> center_priors;
    generate_grid_center_priors(this->input_size, this->input_size, this->strides, center_priors);

    float width_ratio = scale;// (float)image.cols / this->input_size;
    float height_ratio = scale;// (float)image.rows / this->input_size;
    this->decode_infer(out, center_priors, score_threshold, results, width_ratio, height_ratio);
    
    std::vector<BoxInfo> dets;
    for (int i = 0; i < (int)results.size(); i++)
    {
        this->nms(results[i], nms_threshold);

        for (auto box : results[i])
        {
            box.x1 = (box.x1/* - ceil(halfWpad)*/) / scale;
            box.y1 = (box.y1/* - ceil(halfHpad)*/) / scale;
            box.x2 = (box.x2/* - ceil(halfWpad)*/) / scale;
            box.y2 = (box.y2/* - ceil(halfHpad)*/) / scale;
            if (box.x1 < 0)continue;
            if (box.y1 < 0)continue;
            if (box.x2 < 0)continue;
            if (box.x2 < 0)continue;
            dets.push_back(box);
        }
    }
    return dets;
}

void NanoDet_Dancing::draw(cv::Mat& image, std::vector<BoxInfo> boxes)
{
    for (int i = 0; i < boxes.size(); i++)
    {
        const BoxInfo& bbox = boxes[i];
        cv::rectangle(image, cv::Rect(cv::Point(bbox.x1, bbox.y1), cv::Point(bbox.x2, bbox.y2)), cv::Scalar(0, 0, 255),5);
        char text[256];
        sprintf_s(text, "%s %.1f%%", this->labels[bbox.label], bbox.score * 100);
        int baseLine = 0;
        cv::Size label_size = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, 0.4, 1, &baseLine);
        cv::putText(image, text, cv::Point(bbox.x1, bbox.y1), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0));
    }
}

void NanoDet_Dancing::preprocess(cv::Mat& image, ncnn::Mat& in)
{
    const float mean_vals[3] = { 103.53f, 116.28f, 123.675f };
    const float norm_vals[3] = { 0.017429f, 0.017507f, 0.017125f };
    //in.substract_mean_normalize(mean_vals, norm_vals);
}

void NanoDet_Dancing::preprocess(
    cv::Mat& image,
    float dst_size,
    ncnn::Mat& in,
    //float& halfWpad,
    //float& halfHpad,
    float& scale)
{
    cv::Mat debugView = image;
    float w = (float)debugView.cols;
    float h = (float)debugView.rows;
    if (w > h)
    {
        scale = dst_size / w;
        w = dst_size;
        h = h * scale;
    }
    else
    {
        scale = (float)dst_size / h;
        h = dst_size;
        w = w * scale;
    }

    cv::Mat resizeMat;
    cv::resize(debugView, resizeMat, cv::Size(w, h));
    cv::Mat inputModel;
    int Wpad = abs((float)resizeMat.cols - dst_size);
    int Hpad = abs((float)resizeMat.rows - dst_size);

    cv::copyMakeBorder(resizeMat, inputModel, 0, Hpad, 0, Wpad, cv::BORDER_CONSTANT, cv::Scalar::all(0));
    in = ncnn::Mat::from_pixels(inputModel.data, ncnn::Mat::PIXEL_BGR, inputModel.cols, inputModel.rows);
    in.substract_mean_normalize(this->mean_vals, this->norm_vals);
}


void NanoDet_Dancing::decode_infer(
    ncnn::Mat& feats, 
    std::vector<CenterPrior>& center_priors, 
    float threshold, 
    std::vector<std::vector<BoxInfo>>& results, 
    float width_ratio, 
    float height_ratio)
{
    const int num_points = center_priors.size();
    //printf("num_points:%d\n", num_points);

    //cv::Mat debug_heatmap = cv::Mat(feature_h, feature_w, CV_8UC3);
    for (int idx = 0; idx < num_points; idx++)
    {
        const int ct_x = center_priors[idx].x;
        const int ct_y = center_priors[idx].y;
        const int stride = center_priors[idx].stride;

        const float* scores = feats.row(idx);
        float score = 0;
        int cur_label = 0;
        for (int label = 0; label < this->num_class; label++)
        {
            if (scores[label] > score)
            {
                score = scores[label];
                cur_label = label;
            }
        }
        if (score > threshold)
        {
            //std::cout << "label:" << cur_label << " score:" << score << std::endl;
            const float* bbox_pred = feats.row(idx) + this->num_class;
            results[cur_label].push_back(this->disPred2Bbox(bbox_pred, cur_label, score, ct_x, ct_y, stride, width_ratio, height_ratio));
            //debug_heatmap.at<cv::Vec3b>(row, col)[0] = 255;
            //cv::imshow("debug", debug_heatmap);
        }

    }
}

void NanoDet_Dancing::decode_infer(
    ncnn::Mat& cls_pred, 
    ncnn::Mat& dis_pred, 
    int stride, 
    float threshold, 
    std::vector<std::vector<BoxInfo>>& results)
{
    int feature_h = this->input_size / stride;
    int feature_w = this->input_size / stride;

    //cv::Mat debug_heatmap = cv::Mat(feature_h, feature_w, CV_8UC3);
    for (int idx = 0; idx < feature_h * feature_w; idx++)
    {
        const float* scores = cls_pred.row(idx);
        int row = idx / feature_w;
        int col = idx % feature_w;
        float score = 0;
        int cur_label = 0;
        for (int label = 0; label < this->num_class; label++)
        {
            if (scores[label] > score)
            {
                score = scores[label];
                cur_label = label;
            }
        }
        if (score > threshold)
        {
            //std::cout << "label:" << cur_label << " score:" << score << std::endl;
            const float* bbox_pred = dis_pred.row(idx);
            results[cur_label].push_back(this->disPred2Bbox(bbox_pred, cur_label, score, col, row, stride));
            //debug_heatmap.at<cv::Vec3b>(row, col)[0] = 255;
            //cv::imshow("debug", debug_heatmap);
        }

    }
}



BoxInfo NanoDet_Dancing::disPred2Bbox(const float*& dfl_det, int label, float score, int x, int y, int stride, float width_ratio, float height_ratio)
{
    float ct_x = x * stride;
    float ct_y = y * stride;
    std::vector<float> dis_pred;
    dis_pred.resize(4);
    for (int i = 0; i < 4; i++)
    {
        float dis = 0;
        float* dis_after_sm = new float[this->reg_max + 1];
        activation_function_softmax(dfl_det + i * (this->reg_max + 1), dis_after_sm, this->reg_max + 1);
        for (int j = 0; j < this->reg_max + 1; j++)
        {
            dis += j * dis_after_sm[j];
        }
        dis *= stride;
        //std::cout << "dis:" << dis << std::endl;
        dis_pred[i] = dis;
        delete[] dis_after_sm;
    }
    float xmin = (std::max)(ct_x - dis_pred[0], .0f) * 1;
    float ymin = (std::max)(ct_y - dis_pred[1], .0f) * 1;
    float xmax = (std::min)(ct_x + dis_pred[2], (float)this->input_size) * 1;
    float ymax = (std::min)(ct_y + dis_pred[3], (float)this->input_size) * 1;

    //float xmin = (std::max)(ct_x - dis_pred[0], .0f) * width_ratio;
    //float ymin = (std::max)(ct_y - dis_pred[1], .0f) * height_ratio;
    //float xmax = (std::min)(ct_x + dis_pred[2], (float)this->input_size) * width_ratio;
    //float ymax = (std::min)(ct_y + dis_pred[3], (float)this->input_size) * height_ratio;

    //std::cout << xmin << "," << ymin << "," << xmax << "," << xmax << "," << std::endl;
    return BoxInfo{ xmin, ymin, xmax, ymax, score, label };
}

BoxInfo NanoDet_Dancing::disPred2Bbox(const float*& dfl_det, int label, float score, int x, int y, int stride)
{
    float ct_x = (x + 0.5) * stride;
    float ct_y = (y + 0.5) * stride;
    std::vector<float> dis_pred;
    dis_pred.resize(4);
    for (int i = 0; i < 4; i++)
    {
        float dis = 0;
        float* dis_after_sm = new float[this->reg_max + 1];
        activation_function_softmax(dfl_det + i * (this->reg_max + 1), dis_after_sm, this->reg_max + 1);
        for (int j = 0; j < this->reg_max + 1; j++)
        {
            dis += j * dis_after_sm[j];
        }
        dis *= stride;
        dis_pred[i] = dis;
        delete[] dis_after_sm;
    }
    float xmin = (std::max)(ct_x - dis_pred[0], .0f);
    float ymin = (std::max)(ct_y - dis_pred[1], .0f);
    float xmax = (std::min)(ct_x + dis_pred[2], (float)this->input_size);
    float ymax = (std::min)(ct_y + dis_pred[3], (float)this->input_size);

    return BoxInfo{ xmin, ymin, xmax, ymax, score, label };
}

void NanoDet_Dancing::nms(std::vector<BoxInfo>& input_boxes, float NMS_THRESH)
{
    std::sort(input_boxes.begin(), input_boxes.end(), [](BoxInfo a, BoxInfo b) { return a.score > b.score; });
    std::vector<float> vArea(input_boxes.size());
    for (int i = 0; i < int(input_boxes.size()); ++i) {
        vArea[i] = (input_boxes.at(i).x2 - input_boxes.at(i).x1 + 1)
            * (input_boxes.at(i).y2 - input_boxes.at(i).y1 + 1);
    }
    for (int i = 0; i < int(input_boxes.size()); ++i) {
        for (int j = i + 1; j < int(input_boxes.size());) {
            float xx1 = (std::max)(input_boxes[i].x1, input_boxes[j].x1);
            float yy1 = (std::max)(input_boxes[i].y1, input_boxes[j].y1);
            float xx2 = (std::min)(input_boxes[i].x2, input_boxes[j].x2);
            float yy2 = (std::min)(input_boxes[i].y2, input_boxes[j].y2);
            float w = (std::max)(float(0), xx2 - xx1 + 1);
            float h = (std::max)(float(0), yy2 - yy1 + 1);
            float inter = w * h;
            float ovr = inter / (vArea[i] + vArea[j] - inter);
            if (ovr >= NMS_THRESH) {
                input_boxes.erase(input_boxes.begin() + j);
                vArea.erase(vArea.begin() + j);
            }
            else {
                j++;
            }
        }
    }
}
