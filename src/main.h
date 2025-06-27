#ifndef AUTOAIM_MAIN_H
#define AUTOAIM_MAIN_H

#include "utils/include/Config.h"
#include "driver/include/VideoCapture.h"
#include "driver/include/VideoSave.h"
#include "armor_detector/include/Detector.h"
#include "utils/include/Log.h"
#include "Thread.h"
#include "string"


using namespace nw; 
using namespace std;
namespace nw {
    const string config_file_path="../src/utils/tools/init.json"; //配置文件路径
}
#endif // AUTOAIM_MAIN_H