#ifndef VIDEO_SAVE
#define VIDEO_SAVE

#include <opencv2/opencv.hpp>
#include "VideoCapture.h"
#include "Params.h"

using namespace cv;
namespace nw{
    class VideoSave{
        public:
            VideoSave();
            ~VideoSave();
            void SaveVideo(Image** frame_pp);
        private:
            int id;
            float gamma;
            unordered_map<int,float>gamma_table;
            VideoWriter writer;
            VideoWriter weiter_visual;
    };
}
#endif