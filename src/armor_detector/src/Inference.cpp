#include "../include/Inference.hpp"

static constexpr int INPUT_W = 416;    // Width of input
static constexpr int INPUT_H = 416;    // Height of input
static constexpr int NUM_CLASSES = 8;  // Number of classes
static constexpr int NUM_COLORS = 8;   // Number of color
static constexpr int TOPK = 128;       // TopK
static constexpr float NMS_THRESH = 0.3;
static constexpr float BBOX_CONF_THRESH = 0.85;
static constexpr float MERGE_CONF_ERROR = 0.15;
static constexpr float MERGE_MIN_IOU = 0.9;

namespace nw {
    static inline int argmax(const float* data, int size) {
        int max_arg = 0;
        for(int i=0;i<size;i++){
            if(data[i]>data[max_arg]){
                max_arg = i;
            }
        }
       return max_arg;
    }
    inline cv::Mat scaleResize(cv::Mat& img,Eigen::Matrix<float,3,3> &transaction_matrix){
        float r=std::min(INPUT_W/(img.cols*1.0),INPUT_H/(img.rows*1.0));
        int unpad_w=r*img.cols;
        int unpad_h=r*img.rows;

        int dw=INPUT_W-uppad_w;
        int dh=INPUT_H-uppad_h;

        dw/=2;
        dh/=2;
        transform_matrix<<1.0/r, 0,     -dw/r,
                            0,   1.0/r, -dh/r,
                            0,   0,      1.0;
        cv::Mat resized_img;
        cv::resize(img,result_img,cv::Size(unpad_w,unpad_h));
        cv::Mat out;
        cv::copyMakeBorder(resized_img,out,dh,dh,dw,dw,cv::BORDER_CONSTANT);

        return out;
    }
    static void generate_grids_and_stride(const int target_w,const int target_h,
                                         std::vector<int>& strides,std::vector<GridAndStride>& grid_strides) {
        for (auto stride:strides) {
            int num_grid_w = target_w / stride;
            int num_grid_h = target_h / stride;

            for (int g1 = 0; g1 < num_grid_h; g1++) {
                for (int g0 = 0; g0 < num_grid_w; g0++) {
                    GridAndStride grid_stride={g0,g1,stride};
                    grid_strides.emplace_back(grid_stride);
                }
            }
        }
    }

    static void generateYOLOXProposals(std::vector<GridAndStride> grid_strides, const float* feat_ptr,
                                        Eigen::Matrix<float,3,3> &transform_matrix,float prob_threshold,
                                        std::vector<ArmorObject>& objects) {
            const int num_anchors=grid_strides.size();
            for(int anchor_idx = 0; anchor_idx < num_anchors; anchor_idx++){
                const int grid0 = grid_strides[anchor_idx].grid0;
                const int grid1 = grid_strides[anchor_idx].grid1;
                const int stride = grid_strides[anchor_idx].stride;
                const int basic_pos = anchor_idx * (9 + (NUM_COLORS) + NUM_CLASSES);

           // yolox/models/yolo_head.py decode logic
            //  outputs[..., :2] = (outputs[..., :2] + grids) * strides
            //  outputs[..., 2:4] = torch.exp(outputs[..., 2:4]) * strides
            float x_1 = (feat_ptr[basic_pos + 0] + grid0) * stride;
            float y_1 = (feat_ptr[basic_pos + 1] + grid1) * stride;
            float x_2 = (feat_ptr[basic_pos + 2] + grid0) * stride;
            float y_2 = (feat_ptr[basic_pos + 3] + grid1) * stride;
            float x_3 = (feat_ptr[basic_pos + 4] + grid0) * stride;
            float y_3 = (feat_ptr[basic_pos + 5] + grid1) * stride;
            float x_4 = (feat_ptr[basic_pos + 6] + grid0) * stride;
            float y_4 = (feat_ptr[basic_pos + 7] + grid1) * stride;
            
            float box_objectness = (feat_ptr[basic_pos + 8]);
            int box_color = argmax(feat_ptr + basic_pos + 9, NUM_COLORS);
            int box_class = argmax(feat_ptr + basic_pos + 9 + NUM_COLORS, NUM_CLASSES);

            // cout << "output:" << endl;
            // for (int ii = 0; ii < 25; ii++)
            // {
            //     cout << feat_ptr[basic_pos + ii] << " ";
            // }
            // cout << endl;
            // float color_conf = (feat_ptr[basic_pos + 9 + box_color]);
            // float cls_conf = (feat_ptr[basic_pos + 9 + NUM_COLORS + box_class]);
            // float box_prob = (box_objectness + cls_conf + color_conf) / 3.0;
            float box_prob = box_objectness;
            if (box_prob >= prob_threshold)
            {
                ArmorObject obj;

                Eigen::Matrix<float,3,4> apex_norm;
                Eigen::Matrix<float,3,4> apex_dst;

                apex_norm << x_1, x_2, x_3, x_4,
                             y_1, y_2, y_3, y_4,
                             1,   1,   1,   1;
                
                apex_dst = transform_matrix * apex_norm;

                for (int i = 0; i < 4; i++)
                {
                    obj.apex[i] = cv::Point2f(apex_dst(0,i), apex_dst(1,i));
                    obj.pts.push_back(obj.apex[i]);
                }
                
                std::vector<cv::Point2f> tmp(obj.apex, obj.apex + 4);
                // obj.rect = cv::boundingRect(tmp);
                obj.cls = box_class;                                // 得分最大的那一个
                obj.color = box_color;
                obj.prob = box_prob;

                // cout << "output:";
                // for (int i = 0; i < 4; i++)
                // {
                //     cout << " " << "(" << obj.pts[i].x << "," << obj.pts[i].y << ") "; 
                // }
                // cout << "obj_prob:" << obj.prob << " obj_color:" << obj.color << " obj_cls:" << obj.cls << endl;

                objects.push_back(obj);
            }
            }
        }


    
}