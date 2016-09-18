#ifndef _CDECODEPHASE_H_
#define _CDECODEPHASE_H_

#include <opencv2/opencv.hpp>
#include "CVisualization.h"
#include "GlobalFunction.h"
#include "StaticParameters.h"
using namespace cv;

// 相移解码器。解码已经编写好的相移码。
// 输入为一组灰度图，输出为一张灰度图，每一点存储的是projector中的偏移坐标。
class CDecodePhase
{
private:
	int m_numMat;		// 图片数目。默认为2。
	int m_pixPeroid;	// 像素周期。
	int m_resRow;				// 图像的行分辨率
	int m_resLine;			// 图像的列分辨率

	Mat * m_grePicture;	// 输入的灰度图
	Mat m_result;		// 结果

	CVisualization * m_visual;	// 用于显示中间结果

	bool AllocateSpace();		// 为输入的矩阵申请空间
	bool DeleteSpace();			// 删除所有空间
	bool CountResult();			// 根据归一化灰度图统计结果
	bool Visualize();			// 显示中间结果

public:
	CDecodePhase();
	~CDecodePhase();

	bool Decode();
	Mat GetResult();

	bool SetMat(int num, Mat pic);						// 输入相应灰度图
	bool SetNumMat(int numDigit, int pixperiod);			// 设置参数
};

#endif