#ifndef AUTOAIM_GREENLIGHTDETECTOR_HPP
#define AUTOAIM_GREENLIGHTDETECTOR_HPP

#include <iostream>
#include <string>
#include <vector>

#include <opencv2/opencv.hpp>
#include <openvino/openvino.hpp>

using namespace std;
using namespace std;

struct bbox
{
    float x1;
    float y1;
    float x2;
    float y2;
    int class_id;
    float score;
};

namespace nw{
    class GreenLightDetector{
        public:
            GreenLightDetector();
            GreenLightDetector(string model_path);
            ~GreenLightDetector();
            void detect(const Mat img, vector<Rect2f> &rois, Mat &debugImg, double conf_thresh=0.45);
            Rect2f getROI(cv::Mat img,bbox result);
            void VisualzeResult(const cv::Mat &img, bbox &results);
        private:
            const double IMG_SIZE=384;
            std::string model_path="./model/0801_384nano_green_openvino_model/0801_384nano_green.xml";
            float nms_threshold = 0.45f;
            double p_width=1.0f;
            double p_height=1.0f;
            ov::Core ie;
            ov::CompiledModel compiled_model;
            ov::InferRequest infer_request;

            ov::Tensor input_tensor;
            ov::Tensor output_tersor;
            ov::Shape input_shape;
            ov::Shape output_shape;

            const std::vector<std::string> class_names = {"blue","red"};
            Mat letterbox(const cv::Mat& source);
    };
}
#endif //AUTOAIM_GREENLIGHTDETECTOR_HPP