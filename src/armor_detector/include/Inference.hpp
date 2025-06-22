#ifndef INFERENCE_HPP_
#define INFERENCE_HPP_

#include <iterator>
#include <memory>
#include <string>
#include <vector>
#include <iostream>

#include <openvino/openvino.hpp>

#include <opencv2/imgcodecs.hpp>
#include <opencv2/opencv.hpp>

#include <Eigen/Core>

#define ROI_OUTPOST_XMIN 542
#define ROI_OUTPOST_YMIN 588
#define ROI_OUTPOST_WIDTH 416
#define ROI_OUTPOST_HEIGHT 416

#define ROI_OUTPOST_WITH_ROLL_XMIN 632
#define ROI_OUTPOST_WITH_ROLL_YMIN 608

namespace nw {

    struct Armorobject
    {
        cv::Rect<float> rect;
        int cls;
        int color;
        float prob;
        std::vector<cv::Point2f> pts;
        int area;
        cv::point2f apex(4);
    };

    struct GridAndStride
    {
        int grid0;
        int grid1;
        int stride;
    };

    class ArmorDetector{
        public:
            int roi_outpost_xmin = ROI_OUTPOST_XMIN;
            int roi_outpost_ymin = ROI_OUTPOST_YMIN;
            ArmorDetector();
            ~ArmorDetector();
            bool detector(cv::Mat &src,std::vector<ArmorObject>& object,bool use_roi=false);
            void drwaArmors(cv::Mat &src, std::vector<ArmorObject> &object);
        private:
            int dw,dh;
            float rescale_ratio;

            ov::Core core;
            std::shared_ptr<ov::Model> model;
            ov::CompiledModel compiled_model;
            ov::InferenceRequest inference_request;
            ov::Tensor input_tensor;

            std::string input_name;
            std::string output_name;

            Eigen::Matrix<float,3,3> transfrom_matrix;
    };
    
}

#endif //INFERENCE_HPP_