#include "VideoCapture.h"

using namespace nw;
using namespace cv;

void NativeVideo::open(){
    LOG(INFO) << "Attempting to open video file: " << CameraParam::video_path;
    this->video.open(CameraParam::video_path);
    if(!video.isOpened()) {
        LOG(ERROR) << "Video open failed: " << CameraParam::video_path;
    } else {
        LOG(INFO) << "Video open success: " << CameraParam::video_path;
        LOG(INFO) << "Video properties - Width: " << video.get(cv::CAP_PROP_FRAME_WIDTH)
                  << ", Height: " << video.get(cv::CAP_PROP_FRAME_HEIGHT)
                  << ", FPS: " << video.get(cv::CAP_PROP_FPS)
                  << ", Frame count: " << video.get(cv::CAP_PROP_FRAME_COUNT);
    }
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
    
    // 视频循环播放
    while(true){
        DLOG(WARNING) << "                                                              >>>>>  Camera " ;
        umtx_video.lock();
        DLOG(WARNING) << "                                                             umtx_video " ;
        _video_thread_params.video >> *(frame[id].mat);
        
        // 检查是否到达视频结尾
        if(frame[id].mat->empty()) {
            LOG(INFO) << "Video reached end, restarting from beginning...";
            // 重新打开视频文件，从头开始播放
            _video_thread_params.video.set(cv::CAP_PROP_POS_FRAMES, 0);
            _video_thread_params.video >> *(frame[id].mat);
            
            // 如果重新读取后仍然为空，说明视频文件有问题
            if(frame[id].mat->empty()) {
                LOG(ERROR) << "Failed to restart video, trying to reopen file...";
                _video_thread_params.video.release();
                _video_thread_params.video.open(CameraParam::video_path);
                if(!_video_thread_params.video.isOpened()) {
                    LOG(ERROR) << "Failed to reopen video file!";
                    umtx_video.unlock();
                    break;
                }
                _video_thread_params.video >> *(frame[id].mat);
            }
        }
        
        (*_video_thread_params.frame_pp)->mat = frame[id].mat;
        id = (id+1) % size;
        Thread::image_is_update = true;
        Thread::cond_is_update.notify_all();
        DLOG(WARNING) << "                                                              unlock  " ;
        umtx_video.unlock();
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