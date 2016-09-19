#ifndef _CSENSOR_H_
#define _CSENSOR_H_

#include "head.h"

#include <string>
#include <opencv2/opencv.hpp>
#include <strstream>

using namespace std;
using namespace cv;

// 传感器模块。模拟
class CSensor
{
private:
	int m_frameNum;					// 标定的棋盘格帧数。
	int m_ProPicNum[4];				// 各部分Pattern的总计图案数

	string m_chessFilePath;	// 存储文件的总路径
	string m_chessFileFrameNum;	// 当前标定的帧数
	string m_chessFileCamPath;	// 用于相机标定的文件路径
	string m_chessFileCamName;	// 用于相机标定的文件名
	string m_chessFileProPath[4];	// 用于投影仪标定的文件路径
	string m_chessFileProName[4];	// 用于投影仪标定的文件名
	string m_chessFileSuffix;	// 文件后缀名

public:
	CSensor();
	~CSensor();
	bool SetChessFrame(int frame);	// 设定当前标定的帧数	
	Mat GetCamFrame();			// 获取相机图像
	int GetPicNum(int patternIdx);	// 获取投影仪图案的总数
	Mat GetProFrame(int patternIdx, int picIdx);	// 获取投影仪图像
};

#endif