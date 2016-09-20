#ifndef _CSENSOR_H_
#define _CSENSOR_H_

#include <string>
#include <opencv2/opencv.hpp>
#include <strstream>
#include "CCamera.h"
#include "CProjector.h"
#include "StaticParameters.h"
#include "GlobalFunction.h"
#include "CStorage.h"

using namespace std;
using namespace cv;

// 传感器模块。
class CSensor
{
private:
	// 待投影图案存储：
	int m_patternNum;
	int m_nowNum;
	string m_filePath;
	string m_fileName;
	string m_fileSuffix;
	Mat * m_patterns;

	// 设备管理
	CCamera * m_camera;
	CProjector * m_projector;

public:
	CSensor();
	~CSensor();

	// 初始化传感器
	bool InitSensor();

	// 关闭传感器
	bool CloseSensor();

	// 读取图案
	bool LoadPatterns(int patternNum, string filePath, string fileName, string fileSuffix);

	// 释放已读取图案
	bool UnloadPatterns();

	// 设置投影仪投影的图像
	bool SetProPicture(int nowNum);
	
	// 获取相机图像
	Mat GetCamPicture();

	// 获取投影仪投影的图像
	Mat GetProPicture();
	
};

#endif