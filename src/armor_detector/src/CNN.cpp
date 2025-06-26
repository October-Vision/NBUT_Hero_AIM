#include "Classifier.h"

using namespace nw;

CNN::CNN(){
    net=dnn::readNetFromONNX(model);
}

int CNN::predict(const Mat &frame,float& prob, float prob_thrsh){
    Mat blob;
    dnn::blobFromImage(frame,blob,1,size(32,32),Scalar(0,0,0),false,false,CV_32FC1);
    net.setInput(blob);
    Mat predict=net.forward();
    softmax(predict);
    int class_ = 0;
    for(int i=0;i<9;i++){
        if(predict.at<float>(i)>prob){
            prob=predict.at<float>(i);
            class_=i;
        }
    }
    return prob>prob_thrsh ? class_ : 0 ;
}

int CNN::predict(const Mat &frame){
    Mat blob;
    dnn::blobFromImage(frame,blob,1,Size(32,32),Scalar(0,0,0),false,false,CV_32FC1);
    net.setInput(blob);
    Mat predict = net.forward();
    softmax(predict);
    int class_=0;
    float prob=0;
    for(int i=0;i<9;i++){ 
        if(predict.at<float>(i) > prob){
            prob = predict.at<float>(i);
            class_ = i;
        }
    }
    return prob > 0.85 ? class_ : 0;
}

void CNN::train(){

}
void CNN::softmax(Mat& mat){
    float denominator = 0;
    for(int i=0;i<9;i++){
        denominator +=exp(mat.at.<float>(i));
    }
    for(int i=0;i<9;i++){
        mat.at<float>(i)=exp(mat.at<float>(i))/ denominator;
    }
}