#include "../include/VideoCapture.h"
#include "SerialPort.h"
#include "Params.h"

using namespace nw;
using namespace cv;

nw::VideoCapture::VideoCapture(){
    width = CameraParam::width;
    height = CameraParam::height;
    offset_x = CameraParam::offset_x;
    offset_y = CameraParam::offset_y;
}

nw::VideoCapture::~VideoCapture() {

}

void nw::VideoCapture::chooseCameraType(nw::VideoCapture *&video){
    switch(CameraParam::device_type){
        case Dahen:
            video = new DaHenCamera();
            break;
        // case Hik:
        //     video = new HikCamera();
        //     break;
        case Video:
            video = new NativeVideo();
            break;
        case Picture:
            video = new NativePicture();
            break;
        default:
            video=new NativeVideo();
            break;
    }
    video->open();
}