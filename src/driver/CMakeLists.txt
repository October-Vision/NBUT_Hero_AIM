
set(CMAKE_PREFIX_PATH "/usr/loal/inlcude/opencv4")
set(Opencv_DIR /usr/local/include/opencv4)
find_package(OpenCV 4 REQUIRED)

# set(HIKVISION_INCLUDE_DIR /opt/MVS/include/)
# set(HIKVISION_LIB_DIR /opt/MVS/lib/64)

include_directories(
    include
    ../utils/include
    ../armor_detector/include
    ${OpenCV_INCLUDE_DIRS}
    ${HIKDVISION_INCLUDE_DIR}
)

# link_directories(
#     ${HIKVISION_LIB_DIR}
# )

add_library(driver
    src/SerialPort.cpp
    src/Video.cpp
    src/DaHen.cpp
    src/GxCamera.cpp
 #   src/HikCamera.cpp
    src/Picture.cpp
    src/VideoCapture.cpp
    src/VideoSave.cpp
)

target_link_libraries(driver
    utils
    -lpthread
    -lboost_system
    libgxiapi.so
#    MvCameraControl
    ${OpenCV_LIBS}
    
)