#ifndef _CDECODEGRAY_H_
#define _CDECODEGRAY_H_

#include <string>
#include <strstream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "CVisualization.h"
#include "GlobalFunction.h"

using namespace std;
using namespace cv;

// 格雷码解码器。解码已经编写好的格雷码。
// 输入为一组灰度图，输出为一张灰度图，每一点存储的是projector中的坐标。
// 使用前需要分别调用4个Set函数传参。
class CDecodeGray
{
private:
	int m_numDigit;			// 位数
	int m_grayCodeSize;		// 总共的格雷码数目
	short * m_gray2bin;		// 格雷码到二进制码的转换
	string m_codeFilePath;	// 存储格雷码的文件路径
	string m_codeFileName;	// 存储格雷码的文件名
	int resRow;				// 图像的行分辨率
	int resLine;			// 图像的列分辨率
	bool m_vertical;		// 设定格雷码方向

	Mat * m_grePicture;	// 输入的灰度图
	Mat * m_binPicture;	// 加工后的二值图
	Mat m_result;		// 结果

	CVisualization * m_visual;	// 用于显示中间结果

	bool AllocateSpace();		// 为输入的矩阵、short类型内容申请空间
	bool ReleaseSpace();		// 删除所有空间
	bool Grey2Bin();			// 将灰度图加工为二值图，以便进一步处理
	bool CountResult();			// 根据二值图统计结果
	bool Visualize();			// 显示中间结果

public:
	CDecodeGray();
	~CDecodeGray();

	bool Decode();
	Mat GetResult();

	bool SetMat(int num, Mat pic);						// 输入相应灰度图
	bool SetNumDigit(int numDigit, bool ver);				// 设置格雷码位数
	bool SetMatFileName(string codeFilePath,
		string codeFileName);			// 设置存储格雷码的文件名
};

#endif