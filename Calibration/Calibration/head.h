// OpenCV
#include <opencv2/opencv.hpp>

// 其他常用库
#include <vector>
#include <direct.h>

#define VISUAL_DEBUG false
#define CHESS_FRAME_NUMBER 15

// 一些常量
#include "StaticParameters.h"

// 全局性函数
#include "GlobalFunction.h"

// 传感器模块。模拟
#include "CSensor.h"

// 数据存储模块。存储中间数据做存档
#include "CStorage.h"

// 可视化模块，用于debug。自动创建销毁窗口。
#include "CVisualization.h"

// 格雷码解码器。解码已经编写好的格雷码。
// 输入为一组灰度图，输出为一张灰度图，每一点存储的是projector中的坐标。
// 使用前需要分别调用4个Set函数传参。
#include "CDecodeGray.h"

// 相移解码器。解码已经编写好的相移码。
// 输入为一组灰度图，输出为一张灰度图，每一点存储的是projector中的偏移坐标。
#include "CDecodePhase.h"

// 标定类。用于标定摄像头、投影仪。
// 其中会引用摄像头和投影仪的控制，并创建两个解码器进行解码。
#include "CCalibration.h"