#ifndef AUTOAIM_CLASSIFIER_H
#define AUTOAIM_CLASSIFIER_H
#include "opencv2/opencv.hpp"
#include "Log.h"
#include <vector>
using namespace cv;
using namespace std;
namespace nw{
    class Classifier{
        public:
            virtual int predict(const Mat& frame)=0;
            virtual void train()=0;
        private:

    };

    class NSVM : public Classifier{
        public:
            NSVM();
            int predict(const Mat& frame) override;
            void train() override;
            const Mat& getArmor();
           
        private:
            Ptr<ml::SVM> svm;
            HDGDercriptor hog;
            Mat armor;
            vector<float> decs;
    };
    class FSVM : public Classifier{
        public:
            FSVM();
            int predict(const Mat& frame) override;
            void train() override;
            const Mat& getArmor();
        private:
            Ptr<ml::SVM> svm;
            HDGDercriptor hog;
            Mat armor;
            vector<float> decs;
    };
    class CNN :public Classifier{
        pubulic:
            CNN();
            int predict(const Mat& frame) override;
            int predict(const Mat& frame,float &prob,float prob_thresh);
            void train() override;
        private:
            void softmax(Mat& mat);
            string model = "../src/utils/tools/armor.onnx.onnx";
            dnn::Net net;
    };

}
#endif //AUTOAIM_CLASSIFIER_H