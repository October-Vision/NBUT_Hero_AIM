#include <main.h>

using namespace nw; //命名空间
int main(int argc, char *argv[]) {
  Params_ToSerialport parms_to_serialport(&SerialParam::recv_data);
  Params_ToVideo params_to_video;
  Params_ToDetect params_to_detect;
  /********init config**********/
  auto config=new Config(config_file_path);
  config->parse();
  /********init serial port read**********/
  auto serial_port=new SerialPort(SerialParam::device_name);
  thread serial_port_thead(&SerialPort::read_data,ref(params_to_serial_port));
  /*********init camera*********/
  nw::VideoCapture* video;
  video->chooseCameraType(video);
  thread video_thread(&nw::VideoCapture::startcapture,video,rsf(params_to_video));
}