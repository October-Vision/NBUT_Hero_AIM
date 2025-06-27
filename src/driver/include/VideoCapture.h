#ifndef AUTOAIM_VIDEOCAPTURE_H
#define AUTOAIM_VIDEOCAPTURE_H

#include <opencv2/opencv.hpp>

#include "Params.h"
#include "Log.h"
#include "SerialPort.h"
#include "GxCamera.h"
#include "Thread.h"
// #include "HikCamera.h" 
using namespace cv;
namespace nw{
    void *saveFrameToNavtive(void *params_p);
    void *getFrameFromPicture(void *params_p);

    //相机类型
    enum CameraType{
        Dahen,
        Hik,
        Video,
        Picture,
    };

    class Image
    {
        public:
            Mat* mat;
            std::chrono::steady_clock::time_point time_stamp;
            SerialPortData imu_data;
    }; 

    struct Params_ToVideo
    {
        cv::VideoCapture video;        // 相机
        Image **frame_pp;              // Image
        void *__this;
        VideoWriter writer;            // 写入 

        Params_ToVideo()
        {
            frame_pp = (Image **)malloc(sizeof(Image *));
            *frame_pp = new Image();
        }
        ~Params_ToVideo(){
            free(*frame_pp);
            free(frame_pp);
        }
    }; 

    class VideoCapture
    {
        public:
            VideoCapture();
            ~VideoCapture();
            virtual void open() = 0;
            virtual void startCapture(Params_ToVideo &) = 0;
            void startSave(Params_ToVideo &params_to_video);
            void chooseCameraType(VideoCapture *&);

        protected:
            double rate{};
            int _id;
            uint16_t height;
            uint16_t width;
            uint16_t offset_x;
            uint16_t offset_y;

            pthread_t threadID{};
            pthread_t threadID2{};

            Params_ToVideo _video_thread_params;

            VideoWriter writer;

            SerialPort *_serial_port;
            SerialPortData *_data_read;
    }; 

    class DaHenCamera : public VideoCapture
    {
    public:
        explicit DaHenCamera();
        ~DaHenCamera();
        void startCapture(Params_ToVideo &) override;
        void open() override;

    private:
        GxCamera *camera;
    };

    class NativeVideo : public VideoCapture
    {
    public:
        explicit NativeVideo();
        ~NativeVideo();
        void open() override;
        void startCapture(Params_ToVideo &params) override;

    private:
        cv::VideoCapture video;
    };

    class NativePicture : public VideoCapture
    {
    public:
        explicit NativePicture() = default;
        void open() override;
        void startCapture(Params_ToVideo &) override;

    private:
        string base_dir;
        string suffix;
        int id = 0;
    }; 

    // class HikCamera : public VideoCapture
    // {
    //     public:
    //         explicit HikCamera();
    //         ~HikCamera();
    //         void open() override;
    //         void startCapture(Params_ToVideo &) override;
    //     private:
    //         void *cameraHandle;
    //         bool isOpen;
    // }; 
}
#endif //AUTOAIM_VIDEOCAPTURE_H
