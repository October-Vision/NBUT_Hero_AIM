#ifndef __SCORE_
#define __SCORE_
#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>
#define SMALL 1
#define LARGE 2

using namespace std;
using namespace cv;
namespace nw
{
    class Score{
        public:
            Score()= default;
            ~Score() = default;
            void trainSVM(Mat sampleMat, Mat labelMat);

            std::string svmModel = "./model/svm_numbers.xml";
            cv::Ptr<ml::SVM> svm;

        private:
    };
}
#endif // __SCORE_