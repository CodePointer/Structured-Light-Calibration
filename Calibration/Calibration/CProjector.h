#ifndef _CPROJECTOR_H_
#define _CPROJECTOR_H_

#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include "StaticParameters.h"

using namespace std;
using namespace cv;

// 投影仪控制类。用于控制投影仪投射要求的图案。
// 调用：InitProjector，presentPicture，CloseProjector
class CProjector
{
private:
	int m_resRow;			// 投影仪的行分辨率
	int m_resLine;			// 投影仪的列分辨率
	int m_biasRow;			// 窗口的行偏移分辨率
	int m_biasLine;			// 窗口的列偏移分辨率
	string m_winName;		// 窗口名称

public:
	CProjector();			// 构造函数
	~CProjector();			// 析构函数
	bool InitProjector();							// 初始化设备
	bool presentPicture(Mat pic, int time);			// 使投影仪放出pic，延时time
	bool presentPicture(uchar x, int time);			// 使投影仪放出全为x的图片，延时time
	bool CloseProjector();							// 关闭设备
};

#endif