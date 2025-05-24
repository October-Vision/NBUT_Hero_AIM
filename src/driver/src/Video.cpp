#include "VideoCapture.h"

using namespace nw;
using namespace cv;

void NativeVideo::open(){
    this->video.open(CameraParam::video_path);
    LOG_IF(ERROR,!video.isOpened()) << "video open failed"<<CameraParam::video_path;
    LOG_IF(INFO,video.isOpened()) << "video open success"<<CameraParam::video_path;
}

void NativeVideo::startCapture(Params_Tovideo& params){
    //参数输入
    _video_thread_params.video=this->video;
    _video_thread_params.__this=this;
    //参数输出
    _video_thread_params.fream_pp=params.fream_pp;
    int id=0;
    constexpr int size=10;
    Image frame[size];
    for(auto& m:frame)m.mat=new Mat(Size(CameraParam::width,CameraParam::height),CV_32FC3);//初始化大小

    this->video>>*frame[id].mat;
    *_video_thread_params.fream_pp = &frame[id];
    id=(id+1)%size;
    LOG_IF(ERROR,(*_video_thread_params.fream_pp)->mat.empty()) << "frame is empty";

    Mat hsv;
    double alpha=1,beta=30;
    unique_lock<mutex>umtx_video(Thread::mtx_video);
    while(!(*_video_thread_params.fream_pp)->mat->empty()){
        DLOG(WARRIONG)<<"Camera is open";
        umtix_video.lock();
        DLOG(WARRIONG) << "umtx_Video";
        _video_thread_params.video>>*frame[id].mat;
        (*_video_thread_params.fream_pp)->mat=frame[id].mat;
        id=(id+1)%size;
        Thread::image_is_update = true;
        Thread::cond_is_update.notify.all();
        DLOG(WARRING)<<"unlock";
        umtix_video.unlock();
        LOG_IF(ERROR,(*_video_thread_params.fream_pp)->mat->empty())<<"frame is empty";
        DLOG(INFO)<<"end";
        usleep(1000);
    } 
}

void NativeVideo(){

}
NaviteoVideo::~NativeVideo(){
    video.release();
    DLOG(INFO)<<"video release";
}