#include "../include/VideoSave.h"

using namespace nw;

VideoSave::VideoSave(){
    string save_path=CameraParam::video_path.substr(0,CameraParam::video_path.rfind('/')+1);
    FILE* fp=popen(("ls -l"+save_path +" |grep '\\<"+(SerialParam::recv_data.color==101?"red":"blue")+"' |grep ^- | wc -l").c_str(),"r");
    std::fscanf(fp,"%d",&id);
    pclose(fp);
    writer=VideoWriter(save_path+DetectorParam::color + to_string(id)+".avi",VideoWriter:;fource('M','P','4','2'),210.2,Size(CameraParam::width,CameraParam::height));
    LOG(INFO)<<"save video in :"<<save_path+DetectorParam::color+to_string(id)+".avi";
}

VideoSave::~VideoSave(){
    writer.release();
    LOG(INFO)<<"video release";
    writer_visual.release();
}

void VideoSave::SaveVideo(Image** frame_p){
    sleep(1);
    const Mat& frame=*(**frame_p).mat;
    while (!frame.empty())
    {
        writer.write(frame);
    }
    
}