#ifndef AUTOAIM_LIGHTBARFINDER_H
#define AUTOAIM_LIGHTBARFINDER_H

#include <opencv2/opencv.hpp>
#include <vector>

#include "Parmas.h"
#include "Log.h"

using namespace std;
using namespace cv;
namespace nw {
    typedef vector<RotatedRect> LightBarBlobs;
    class LightBarFinder {
        public:
            LightBarFinder();
            bool findLightBarBlobs(const Mat& img, LightBarBlobs& lightBarBlobs);
            static inline bool isVaildLightBarBlob(const RotgatedRect&)
        private:
        static inline bool checkAspectRatio(double);
        static inline bool checkArea(double);
        static inline bool checkAngle(const RotatedRect&);

        Mat kernel1;
        Mat kernel2;
    };
}
#endif // AUTOAIM_LIGHTBARFINDER_H