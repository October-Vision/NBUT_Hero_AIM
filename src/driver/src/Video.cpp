#include "VideoCapture.h"

using namespace nw;
using namespace cv;

void NativeVideo::open(){
    this->video.open(CameraParam::video_path);
    LOG_IF(ERROR,!video.isOpened()) << "video open failed"<<CameraParam::video_path;
    LOG_IF(INFO,video.isOpened()) << "video open success"<<CameraParam::video_path;
}

void NativeVideo::startCapture(Params_ToVideo& params){
    // params in
    _video_thread_params.video = this->video;
    _video_thread_params.__this = this;

    // params out
    _video_thread_params.frame_pp = params.frame_pp;

    int id = 0;
    constexpr int size = 10;
    Image frame[size];
    for(auto& m: frame) m.mat = new Mat(Size(1280, 1024), CV_32FC3);

    this->video >> *frame[id].mat;
    *_video_thread_params.frame_pp = &frame[id];
    id = (id+1) % size;
    LOG_IF(ERROR, (*_video_thread_params.frame_pp)->mat->empty()) << "get empty picture mat!";

    Mat hsv;
    double alpha = 1, beta = 30;
    unique_lock<mutex> umtx_video(Thread::mtx_image, defer_lock);
    while(!(*_video_thread_params.frame_pp)->mat->empty()){
        DLOG(WARNING) << "                                                              >>>>>  Camera " ;
        umtx_video.lock();
        DLOG(WARNING) << "                                                             umtx_video " ;
        _video_thread_params.video >> *(frame[id].mat);
        (*_video_thread_params.frame_pp)->mat = frame[id].mat;
        id = (id+1) % size;
        Thread::image_is_update = true;
        Thread::cond_is_update.notify_all();
        DLOG(WARNING) << "                                                              unlock  " ;
        umtx_video.unlock();
        LOG_IF(ERROR, (*_video_thread_params.frame_pp)->mat->empty()) << "get empty picture mat!";
        DLOG(WARNING) << "                                                               end  " ;
        usleep(10000);
    }
}

NativeVideo::NativeVideo() {
    
}

NativeVideo::~NativeVideo() {
    video.release();
//    writer.release();
}