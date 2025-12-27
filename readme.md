## 简介
 用于宁波工程学院英雄自瞄
## 编译方式
mkdir build
cd build
cmake ..
make

# 新增视频模式

### 1. 虚拟串口模式
修改了 [`SerialPort`](src/driver/src/SerialPort.cpp) 类以支持虚拟串口：

#### 主要修改点：
- **构造函数**: 当无法连接真实串口时，自动切换到虚拟串口模式（`_serial_port = nullptr`）
- **readData()**: 在虚拟模式下返回全0的IMU数据
  - `yaw = 0`
  - `pitch = 0`
  - `roll = 0`
  - `color = 101` (默认红色)
  - 其他字段均为0
- **serialPortRead()**: 虚拟模式下不执行实际读取
- **serialPortWrite()**: 虚拟模式下不执行实际写入

### 2. 配置文件修改
修改了 [`init.json`](src/utils/tools/init.json:5) 配置文件：
```json
{
  "camera": {
    "device_type": 2,  // 改为2启用视频模式
    "video_path": "../src/utils/data/video/test.avi"  // 指向新的视频文件夹
  }
}
```

### 3. 视频加载逻辑
[`VideoCapture::chooseCameraType()`](src/driver/src/VideoCapture.cpp:19) 已正确支持视频模式：
- `device_type = 0`: 大恒相机
- `device_type = 1`: 海康相机
- `device_type = 2`: 视频文件 
- `device_type = 3`: 图片序列

## 使用方法

### 准备视频文件
将测试视频放入 `src/utils/data/video/` 文件夹，例如：
```bash
cp your_test_video.avi src/utils/data/video/test.avi
```

### 配置
编辑 `src/utils/tools/init.json`：
```json
{
  "camera": {
    "device_type": 2,
    "video_path": "../src/utils/data/video/test.avi"
  }
}
```

### 虚拟串口实现
```cpp
// 在SerialPort构造函数中
catch (...) {
    LOG(WARNING) << "Failed to connect to real serial port, using virtual serial port mode";
    this->_serial_port = nullptr;  // 标记为虚拟模式
}

// 在readData中
if (_serial_port == nullptr) {
    // 返回全0的IMU数据
    imu_data->yaw = 0;
    imu_data->pitch = 0;
    imu_data->roll = 0;
    // ...
    return;
}
```

### 视频读取流程
1. [`main.cpp`](src/main.cpp:18) 调用 `chooseCameraType()` 选择相机类型
2. 根据 `device_type = 2` 创建 [`NativeVideo`](src/driver/src/Video.cpp:6) 对象
3. [`NativeVideo::open()`](src/driver/src/Video.cpp:6) 打开视频文件
4. [`NativeVideo::startCapture()`](src/driver/src/Video.cpp:12) 循环读取视频帧


## 切换模式

### 切换到视频模式
```json
{"camera": {"device_type": 2}}
```

### 切换回相机模式
```json
{"camera": {"device_type": 0}}  
```

## 文件修改清单

1. `src/driver/src/SerialPort.cpp`(src/driver/src/SerialPort.cpp) - 添加虚拟串口支持
2. `src/utils/tools/init.json`(src/utils/tools/init.json) - 修改配置为视频模式
3. `src/utils/data/video/` - 创建视频文件夹
4. `src/utils/data/video/README.md` - 添加使用说明
5.  编译测试通过
