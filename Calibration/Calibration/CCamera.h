#ifndef _CCAMERA_H_
#define _CCAMERA_H_

#include <opencv2/opencv.hpp>
#include <Windows.h>	// 摄像头驱动需要
#include <HVDAILT.h>
#include <HVDef.h>
#include <HVExtend.h>
#include <Raw2Rgb.h>	// 摄像头驱动需要
#include "StaticParameters.h"


using namespace std;
using namespace cv;

// 摄像头控制类。获取一帧摄像头图片，并实现对摄像头的管理（打开、关闭等等）
// 默认是只有一个摄像头。
// 调用：InitCamera，getPicture，CloseCamera
class CCamera
{
private:
	int sumDeviceNum;			// 总共的摄像头数目
	int nowDeviceNum;			// 当前使用的摄像头序号。无摄像头则为0。
	HHV DeviceHandle;			// 摄像头设备句柄
	BYTE **ppBuffer;			// 图像缓冲区
	int BufferNum;				// 图像缓冲区大小，即图片张数
	int used;					// 缓冲区已用图片张数
	int resRow;					// 图片分辨率：行
	int resLine;				// 图片分辨率：列


public:
	CCamera();
	~CCamera();
	int getSumDeviceNum();		// 获取总共摄像头数目
	int getNowDeviceNum();		// 当前摄像头。
	bool InitCamera();			// 初始化设备以备采集
	bool SnapShot();
	bool getPicture(Mat & pic);	// 获取一帧图像，并存在pic中。
	bool CloseCamera();			// 关闭摄像头，终止采集
};

#endif