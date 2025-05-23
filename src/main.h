#ifndef MAIN_H
#define MAIN_H

#include "utils/include/config.h"
#include "armor_detector/include/Detector.h"
#include "Thread.h"
#include "string"

using namespace nw; //命名空间
using namespace std; //命名空间
namespace nw {
    const string config_file_path="../src/utils/tools/init.json"; //配置文件路径
}
#endif // MAIN_H
// 头文件保护宏，防止重复包含