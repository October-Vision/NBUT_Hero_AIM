#include <main.h>

using namespace nw; //命名空间
int main(int argc, char *argv[]) {
  Params_ToSerialport parms_to_serialport(&SerialParam::recv_data);
  Params_ToVideo params_to_video;
  Params_ToDetect params_to_detect;
}