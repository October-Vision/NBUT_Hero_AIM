#ifndef HIKCAMERA_H
#define HIKCAMERA_H

#include "VideoCapture.h" // 基类头文件
#include "MvCameraControl.h" // 海康 SDK 的头文件

namespace nw {

class HikCamera : public VideoCapture {
public:
    explicit HikCamera(); // 构造函数
    ~HikCamera();         // 析构函数

    void open() override; // 打开相机
    void startCapture(Params_ToVideo &params) override; // 开始捕获

private:
    void *cameraHandle; // 海康相机句柄
    bool isOpen;        // 标记相机是否已打开
};

} // namespace nw

#endif // HIKCAMERA_H