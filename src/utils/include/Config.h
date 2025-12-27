#ifndef AUTOAIM_CONFIG_H
#define AUTOAIM_CONFIG_H
#include <string>
#include <fstream>

#include "json/json.h"
#include "Params.h"

using namespace std;
namespace nw{
    /*
    解析配置文件
    */
   class Config{
    public:
        Config() = default;
        explicit Config(const string &path);
        void parse();
        void chooseCameraType();
    private:
        string json_file_path;
    };
}
#endif // AUTOAIM_CONFIG_H