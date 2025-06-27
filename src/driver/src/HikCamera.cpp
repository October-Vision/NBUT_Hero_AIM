// #include "MvCameraControl.h" 
//  #include "HikCamera.h" 

// HikvisionCamera::HikvisionCamera() : cameraHandle(nullptr), isOpen(false) {}
// HikvisionCamera::~HikvisionCamera() {
//     if (isOpen) {
//         MV_CC_DestroyHandle(cameraHandle);
//     }
// }

// void HikvisionCamera::open() {
//     int ret = MV_CC_CreateHandle(&cameraHandle, nullptr); 
//     if (ret != MV_OK) {
//         throw std::runtime_error("Failed to create Hikvision camera handle");
//     }

//     ret = MV_CC_OpenDevice(cameraHandle); 
//     if (ret != MV_OK) {
//         MV_CC_DestroyHandle(cameraHandle);
//         throw std::runtime_error("Failed to open Hikvision camera");
//     }

//     isOpen = true;
// }

// void HikvisionCamera::startCapture(Params_ToVideo &params) {
//     if (!isOpen) {
//         throw std::runtime_error("Camera is not open");
//     }

//     int ret = MV_CC_StartGrabbing(cameraHandle); 
//     if (ret != MV_OK) {
//         throw std::runtime_error("Failed to start capturing on Hikvision camera");
//     }
//     MV_FRAME_OUT frame = {0};
//     ret = MV_CC_GetImageBuffer(cameraHandle, &frame, 1000); 
//     if (ret == MV_OK) {
//     } else {
//         throw std::runtime_error("Failed to get image buffer");
//     }
// }