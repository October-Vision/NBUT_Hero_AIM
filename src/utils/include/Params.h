#ifndef AUTOAIM_PARAMS_H
#define AUTOAIM_PARAMS_H
#include <string>
#include "../../driver/include/SerialPort.h"

using namespace std;

namespace nw{
    class CameraParam
    {
    public:
        static int device_type;
        static string sn,video_path,picture_path;
        static int ecposure_time;
        static double gain;
        static float gamma;
        static double yaw;
        static double pitch;
        static double roll;

        static int camera_type;
        static double fx,fy,u0,v0,k1,k2,k3,p1,p2;
        static double camera_trans_x;
        static double camera_trans_y;
        static double camera_trans_z;

        static double height;
        static double width;
    private:
    };

    class Detectorparam{
        public:
            static string color;
            static string thresh;
        private:
    };

    class FilterParams
    {
        public:
            //测量噪声，速度比这个大两倍
            static float measurement_noise_pose_x;
            static float measurement_noise_pose_y;
            static float measurement_noise_pose_z;

            static float process_noise_pose_x;
            static float process_noise_pose_y;
            static float process_noise_pose_z;

            static float process_noise_q4_w;
            static float process_noise_q4_x;
            static float process_noise_q4_y;
            static float process_noise_q4_z;

            static float stf_beta;
            static bool is_use_stf;

            static bool is_use_ca_model;
            //singer模型
            static bool is_use_singer;
            static float alpha;
            static float max_a_x;
            static float max_a_y;
            static float max_a_z;
        private:
    };

    class OutpostParam{
        public:
            static double time_bias;
            static double time_bias_inverse;
            static double center_ratio;
            static double pitch_bias;
        private:
    };

    class SerialParam{
        public:
            static bool enable;
            static string device_name;
            static SerialPortData recv_data;
            static SerialPortWriteData send_data;  
            static vector<string> send_data_sets;
            static int set_id;
        private:
    };
    class GlobalParam{
        public:
            static int MODE;
            static int COLOR;
            static int BIGID;
            static bool DEBUG_MODE;
            static bool SAVE_VIDEO;
            static bool SAVE_ARMOR;
            static bool SHOW_THRESH;
            static int save_step;
            static bool SHOW_COORD;
            static double SHOOT_SPEED;
            static bool SOCKET;
        private:
    };

    typedef enum{
        AUTOAIM,
        ANTITOP,
        OUTPOST,
        GREEN_SHOOT,
    }STATE;
    
    typedef enum{
        HERO,
        SENTRY,
        ENGINEER,
    }PRI;
    
    class StateParam{
        public:
            static STATE state;
    };

    class TimeSystem{
        public:
            static std::chrono::steady_clock::time_point start_time;
        private:
    };
}
#endif // AUTOAIM_PARAMS_H