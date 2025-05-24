#include "Thread.h"

namespace nw{

    bool Thread::image_is_update = false;
    condition_variable Thread::cond_is_update;
    condition_variable Thread::cond_is_process;

    mutex Thread::mtx;
    mutex Thread::mtx_video; // 视频保存和检测的互斥锁
    mutex Thread::mtx_image; // 相机线程锁

} 