#ifndef AUTOAIM_THREAD_H
#define AUTOAIM_THREAD_H

#include<thread>
#include<mutex>
#include<condition_variable>
#include<atomic>

using namespace std;

namespace nw{
    class Thread{
        public:
            static bool image_is_update;
            static condition_variable cond_is_update;
            static condition_variable cond_is_process;

            static mutex mtx;//静态互斥锁
            static mutex mtx_video; //视频保存 detector的互斥锁
            static mutex mtx_image; //相机线程锁 
        private:
            Thread() = default;
    };
}
#endif