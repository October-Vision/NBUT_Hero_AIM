#include "Detector.h"
#include "SerialPort.h"
#include "Inference.hpp"

namespace nw{
    void Detector::calcGammaTable(flaot gamma){
        for (int i = 0; i < 256; ++i)
        {
            gamma_table[i] = saturate_cast<uchar>(pow((float)(i / 255.0), gamma) * 255.0f);
        }
    }
    /*
    参数设置
     */
    void Detector::setParams(const Params_ToDetector &params, SerialPort *SerialPort_){
        Mat drawing=Mat()
        roi_accelerator = new ROIAccelerator();    // ROI 加速
        target_chooser = new TargetChooser();
        auto solver = new PoseSolver();
        // auto predictor = new Predictor();
        auto bumper = new Bumper();                // 缓冲器 
        int last_frame_class = 0;                      // 上一次选中的目标
        int this_frame_class = 0;
        bool right_clicked = false;                   // 用于检测是否右击
        bool last_right_clicked = false;

        auto armorDetector = new ArmorDetector();
        auto greenDetector = new GreenLightDetector();
        //auto greenDetector = new YOLOv10Detector("./model/0715_v10_416n_openvino_model/0715_v10_416n.xml", 0.20);
        string network_path = "./model/best_06_02.xml";
        armorDetector->initModel(network_path);
        if(GlobalParam::DEBUG_MODE) cv::nameWindow("frame",0);
        auto start = std::chrono::steady_clock::now();
        // auto virtual_start = std::chrono::steady_clock::now();          // 记录一下，和图片同步
        TimeSystem::time_zero = std::chrono::steady_clock::now();
        SerialPortData recv_data(SerialParam::recv_data);
        sleep(1);

        double delta_t =0;
        bool first=false;//右键检测

        Point2f center_pixel = cv::Point2f(0,0);
        const string target_mode_str[4] = {"NOT_GET_TARGET", "CONTINOUS_GET_TARGET", "LOST_BUMP", "DETECT_BUMP"};

        enum COLOR{I_AM_BLUE, I_AM_RED};
        while (!(*_detector_thread_params.frame_pp)->mat->empty()){
             // 独占的互斥锁， 实现资源的独立访问
            // umtx_video 是对象名, 用于在当前作用域内管理Thread::mtx_image互斥锁
            // Thread::mtx_image 名为mtx_image的互斥锁
            // mtx_image 是共享资源
            // 有线程占用mtx_image, 导致在这里堵塞
            // umtx_video是管理互斥锁的工具，mtx_image是那个互斥锁
            unique_lock<mutex> umtx_video(Thread::mtx_image);
            
            // DLOG(INFO) << "   mutex   " << std::endl; 
            
            while (!Thread::image_is_update)                            // 图片还没有更新
            {
                Thread::cond_is_update.wait(umtx_video);
            }

            // virtual_start = std::chrono::steady_clock::now();           // 

            // DLOG(INFO) << "begin Detector" << std::endl; 
            const Image &image = **_detector_thread_params.frame_pp;    // 包含时间戳
            Thread::image_is_update = false;                            // 写成false
            Thread::cond_is_process.notify_one();
            umtx_video.unlock();
            const Mat &frame = *image.mat;
            if (abs(SerialParam::recv_data.yaw - recv_data.yaw) < 18000 && abs(SerialParam::recv_data.pitch - recv_data.pitch) < 18000)
            {
                recv_data = SerialParam::recv_data;
            }

            if (GlobalParam::DEBUG_MODE){   // 拷贝一次drawing
                frame.copyTo(drawing);
            }

            switch (SerialParam::recv_data.flag)
            {
            case 0x06:
                 if(StateParam::state != OUTPOST)               // 切换进入前哨战模式
                    {
                        solver->clearCircle();
                    }
                    StateParam::state = OUTPOST;
                break;
//            case 0x07:
//                   if(StateParam::state != SENTINEL){  
//                       solver->clearSentinel();
//                    }
//                   StateParam::state = AUTOAIM_WITH_ROI;
//                     break;
            case 0x05:
                StateParam::state=AUTOAIM;
                break;
            case 0x07:
                StateParam::state=ANTITOP;
                break;
            case 0x08:
                StateParam::state=GREEN_SHOOT;
                break;
            default:
                StateParam::state=AUTOAIM;//默认辅瞄
                break;
            }
            //调试模式
            /*
            1:辅瞄
            2：前哨战
            3：半速前哨
            4：ROI辅瞄
            5：反陀螺
            6：绿灯吊射*/
            switch (GlobalParam::MODE)
            {
            case 1:
                StateParam::state=AUTOAIM;
                break;
            case 2:
                StateParam::state=OUTPOST;
                break;
            case 3:
                StateParam::state=Half_OUTPOST;
                break;
            case 4:
                StateParam::state=AUTOAIM_WITH_ROI;
                break;
            case 5:
                StateParam::state=ANTITOP;
                break;
            case 6:
                StateParam::state=GREEN_SHOOT;
                break;
            default:
                break;
            }
        SerialParam::send_data.shootStatus=0;
        vector<Armorobject> object;
        bool getTarget = false;
        vector<Rect2f> greenObjects;
        if(StateParam::state==OUTPOST || StateParam::state==Half_OUTPOST || StateParam::state==AUTOAIM_WITH_ROI || StateParam::state==ANTITOP){
                // 去除
                if(abs(recv_data.roll) > 500) {
                    armorDetector->roi_outpost_xmin = ROI_OUTPOST_WITH_ROLL_XMIN;
                    armorDetector->roi_outpost_ymin = ROI_OUTPOST_WITH_ROLL_YMIN;
                }
                else {
                    armorDetector->roi_outpost_xmin = ROI_OUTPOST_XMIN;
                    armorDetector->roi_outpost_ymin = ROI_OUTPOST_YMIN;
                }

                cv::Mat img = frame(Rect(armorDetector->roi_outpost_xmin, armorDetector->roi_outpost_ymin, ROI_OUTPOST_WIDTH, ROI_OUTPOST_HEIGHT));
                getTarget=armorDetector->detect(img, objects, true);      // 将点重新映射到桌面上
            }
            else if(StateParam::state==AUTOAIM){
                cv::Mat img = frame.clone();
                // 测时间
//                auto start = std::chrono::steady_clock::now();
                getTarget=armorDetector->detect(img, objects);
//                auto end = std::chrono::steady_clock::now();
//                auto diff=std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
//                 DLOG(INFO) << "detect time: " << diff << "ms";
            }
            else if(StateParam::state==GREEN_SHOOT) {
                auto start = std::chrono::steady_clock::now();
//                greenObjects=greenDetector->RunInference(frame);
                greenDetector->detect(frame, greenObjects, drawing);
                auto end = std::chrono::steady_clock::now();
                auto diff=std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
                DLOG(INFO) << "detect time: " << diff << "ms";
                if(GlobalParam::DEBUG_MODE) {
                    for (auto &greenObject : greenObjects) {
                        cv::rectangle(drawing, greenObject, cv::Scalar(0, 255, 0), 2);
                    }
                }

            }
            if(GlobalParam::DEBUG_MODE){
                armorDetector->drwaArmors(drawing, objects);
                if(StateParam::state==OUTPOST || StateParam::state==Half_OUTPOST || StateParam::state==AUTOAIM_WITH_ROI || StateParam::state==ANTITOP)                           // 绘制出ROI的框
                    cv::rectangle(drawing, Rect(ROI_OUTPOST_XMIN, ROI_OUTPOST_YMIN, ROI_OUTPOST_WIDTH, ROI_OUTPOST_HEIGHT), cv::Scalar(0,0,255),2);
            }
            ArmorBlobs armors;
            ArmorBlob armor;
            for(Armorobject object::objects){
                armor.corners[0] = object.apex[0]; 
                armor.corners[1] = object.apex[3];
                armor.corners[2] = object.apex[2];
                armor.corners[3] = object.apex[1];
                armor.confidence = object.prob;
                armor._class=classifier.predict(*image.mat,armor.corners,1).first;
                armor.rect=cv::boundingRect(armor.corners);
//              if(((object.color / 2) == GlobalParam::COLOR) && (object.cls != 6) && (object.cls != 7))         // 过滤掉己方的颜色 0 是蓝色, 1是红色, 前哨战全部加入
//                    continue;
                armors.emplace_back(armor);
            }
            ArmorBlobs armor_targets;
            // 首先初始化要发的信息
            // SerialParam::send_data.pitch = SerialParam::recv_data.pitch;
            // SerialParam::send_data.yaw = SerialParam::recv_data.yaw;
            bool getCenter = false;
             if(StateParam::state==OUTPOST || StateParam::state==Half_OUTPOST){
                for(int i = 0;i<armors.size();i++) {
                    DLOG(INFO)<<"detec armor id: "<<armors[i]._class;
                    if (armors[i]._class == 7 || armors[i]._class == 2)
                        armor_targets.emplace_back(armors[i]);
                }
            //计算时间差
            delta_t=std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()-start).count()/1000.0f;//m
            start=std::chrono::steady_clock;
            center_pixel =solver->outpostMode(armor_targets, delta_t / 1000, recv_data, SerialPort_, getTarget);
            }else if(StateParam::state==ANTITOP) {
                DLOG(WARNING)<<"recv_yaw: "<<recv_data.yaw<<" recv_pitch: "<<recv_data.pitch<<" recv_roll: "<<recv_data.roll<<endl;
                for(int i = 0;i<armors.size();i++) {
                    DLOG(INFO)<<"armors id: "<<armors[i]._class<<endl;
                    if (armors[i]._class == 7 || armors[i]._class == 2)
                        armor_targets.emplace_back(armors[i]);
                }
                delta_t = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() / 1000.0f;    //ms
                center_pixel=solver->antiTop(armor_targets, delta_t / 1000, recv_data, SerialPort_, getCenter);
                getTarget = getCenter;
            }
            else if(StateParam::state==GREEN_SHOOT) {
                solver->greenShoot(greenObjects, delta_t / 1000, recv_data, SerialPort_, getCenter);
                getTarget = getCenter;
            }else{
                //辅瞄
                for(int i=0;i<armor.size();i++) armor_targets.emplace_back(armor[i]);
                delta_t = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() / 1000.0f;    //ms
                bool useful=false;
                if(armor_targets.size()) useful = solver->getPoseInCamera(armor_targets, delta_t / 1000, recv_data, SerialPort_, this_frame_class, last_frame_class); 
                getTarget = useful;
                if(useful) start=std::chrono::steady_clock::now()
                // -70.24 -97.17
                // 26.57
                // 接下来是辅瞄模式的选择
                // right_clicked = recv_data.right_clicked; // 右击
                // if(last_right_clicked == 0 && right_clicked == 1)
                //     first = true;
                
                // 检测到右击，选取中心的目标进行
                // if(first){
                    // if(armors.size() < 1)
                        // continue;   // 
                    // first = false;

                    // 根据距离中心最近的装甲板进行排序
                    // sort(armors.begin(), armors.end(), [](const ArmorBlob &a, const ArmorBlob &b) -> bool { const Rect& r1 = a.rect;const Rect& r2 = b.rect;
                        // return abs(r1.x+r1.y+r1.height/2+r1.width/2-1024/2-1280/2) < abs(r2.x+r2.height/2-1024/2+r2.y+r2.width/2-1280/2); 
                    // });
                    
                    // 选择最靠近中心的坐标
                    // ArmorBlob center_armor = armors.at(0);
                    // int top_pri = center_armor._class;       // 类别
                    // 最中心的装甲板优先级最高
                    // target_chooser->setTopPri(top_pri);      // 每次操作手右击时，选择中心的装甲板作为优先级最高的类别。
                    // last_frame_class = top_pri;
                // }

                // 选择命中的目标（目标类别的装甲板）
                
                // 得到目标类别
                // armor_targets = target_chooser->getAimTarget(armors, last_frame_class);
                // int indice = target_chooser->getTargetIndice();
                // int target_class_ = armor_targets[indice]._class;

                // this_frame_class = target_class_;           // 本次识别的类别
                // DLOG(WARNING) << "target size: "<< armor_targets.size() << std::endl;

                // // 暂时不用ROI
                // if(armor_targets.size() > 0){
                //     // DLOG(WARNING) << "index " <<indice << std::endl;
                //     roi_accelerator->ROI_create(armors[indice].corners);
                //     // DLOG(WARNING) << armors[indice].corners <<std::endl;
                //     if(GlobalParam::DEBUG_MODE)
                //         roi_accelerator->drawROI(drawing);
                //     // DLOG(WARNING) << roi_accelerator->getRoiOffset() << std::endl;
                // }
                // else{
                //     roi_accelerator->ROI_destroy();
                // }

                // 检测到的类别
                // DLOG(WARNING) << target_class_ << std::endl;

                // 进入缓冲区，判定当前所属的检测模式，eg: 连续识别等
                // int detect_mode = bumper->getDetectMode(this_frame_class, last_frame_class);
                // DLOG(ERROR) << "DETECT MODE:" << target_mode_str[detect_mode];

                // 未检测到、检测缓冲、丢失缓冲
                // if(detect_mode == NOT_GET_TARGET || detect_mode == DETECT_BUMP || detect_mode == LOST_BUMP){
                    // solver->clearCircle();  // 重置预测器
                // }


                // delta_t = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() / 1000.0f;
                // start = std::chrono::steady_clock::now();

                // Angle_t angle_t = predictor->Predict();
                // 更新上一帧对准的类别
                // last_frame_class = this_frame_class;
            }
///////////////////////////////////////////////////////////////////////////////////////////////
///// 以下内容是进入不同的预测器  前哨战模式不需要卡尔曼滤波，不需要预测，所以时间要求不那么高，但是辅瞄需要
///////////////////////////////////////////////////////////////////////////////////////////////          
            // 当前时间减去start时间
            // 计算处理图像的时间 单位为s
            // 如果没有目标不会更新到这里，这里只是粗略的计算时间，按正常来说，进行卡尔曼滤波的部分应该
            
            // 距离上一次的时间，两次检测之间的时间间隔
            // delta_t = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start).count() / 1000.0f;
            // start = std::chrono::steady_clock::now();
            // DLOG(INFO) << "Delta_t " << delta_t << "(ms)";
            
            // DLOG(INFO) << "processing one frame for " << delta_t << "(ms)";
            // 根据现有的模式状态 进行不同的模式
            // TODO 重点关注前哨站模式

            // float delta_t_by_chrono = std::chrono::duration_cast<std::chrono::microseconds>(frame.time_stamp - last_time_point).count() / 1000.0f;
            // last_time_point = frame.time_stamp; //更新时间戳

            // 选择当前的模式
            // switch (StateParam::state){
            //     // 击打前哨站模式 center_pixel 前哨站中心的重投影坐标
            //     case OUTPOST:
            //         center_pixel = solver->outpostMode(armors, delta_t / 1000, recv_data, SerialPort_); break;
                // 原本的哨兵模式  半速前哨站模式
                // case Half_OUTPOST:       
                    // center_pixel = solver->halfoutpostMode(armors, delta_t / 1000, recv_data, SerialPort_); break;
                // solver 存放前哨站中心装甲板
                // default: 
                // 普通辅瞄，跟随 + 卡尔曼滤波
                    // solver->getPoseInCamera(armors, delta_t / 1000, recv_data, SerialPort_, this_frame_class, last_frame_class); break;
                    // shootAngleTime = predictor->Predict(armor_pose, armor_pose_sec, is_get_second_armor, detect_mode, _detector_thread_params.SerialPortData_, delta_t_by_chrono);

            // }
        SerialParam::send_data.state=StateParam::state;
        // 写入数据的时间戳
        SerialParam::send_data.time_stamp = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - TimeSystem::time_zero).count() / 10.0;
        DLOG(INFO) << "                                           recv yaw: " << SerialParam::recv_data.yaw << "  recv pitch: " << SerialParam::recv_data.pitch<<" recv roll: "<<SerialParam::recv_data.roll;
//            getTarget = false;
            if(getTarget){
                DLOG(WARNING)<<"                         !!!!SEND";
                DLOG(INFO) << "                                           send yaw: " << SerialParam::send_data.yaw << " send pitch: " << SerialParam::send_data.pitch;

                SerialPort_->writeData(&SerialParam::send_data);
            } else{
                DLOG(WARNING)<<"                         !!!!SEND";
                DLOG(INFO) << "send yaw: " << SerialParam::send_data.yaw << " send pitch: " << SerialParam::send_data.pitch;
                // 发送数据
                // SerialPort_->writeData(&SerialParam::send_data);
                SerialParam::send_data.shootStatus = 0;
                SerialParam::send_data.yaw = 0x00;
                SerialParam::send_data.pitch = 0x00;
                SerialParam::send_data.num=0x00;
                SerialParam::send_data.time_stamp = 0x00;
                SerialPort_->writeData(&SerialParam::send_data);
                DLOG(INFO)<<"没有目标: "<<SerialParam::send_data.shootStatus<<std::endl;
            }
            if(GlobalParam::DEBUG_MODE){
                if(StateParam::state == OUTPOST || StateParam::state == Half_OUTPOST||StateParam::state == ANTITOP || StateParam::state == GREEN_SHOOT){
                    Scalar color(10,10,255);
                    putText(drawing, "center", Point2f(center_pixel.x,center_pixel.y), FONT_HERSHEY_COMPLEX, 2, Scalar(0,0,255), 4, LINE_8);
                    circle(drawing, center_pixel, 10, color,-1);
                }
                // 展示图像
                imshow("frame", drawing);
                waitKey(1);
            }
            // DLOG(INFO) << "end Detector";
            // if (GlobalParam::SOCKET)
            // {
            //     delta_t_frame.delta_t = delta_t;
            //     udpsender->send(delta_t_frame);
            // }
        }
        destroyAllWindows();
        }
    Detector::Detector(){
        // if(GlobalParam::SOCKET){
        //     //构建socket连接，端口号4000检测delta_t
        //     udpsender = new UDPSender("192.168.1.3",4000)
        // }
}
}