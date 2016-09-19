#include "CSensor.h"


// 传感器模块。
// 用于模拟传感器传入数据。
// 由于标定内容之前已经离线采集好，因此这个类只需要从文件中读入文件即可。

CSensor::CSensor()
{
	this->m_frameNum = CHESS_FRAME_NUMBER;
	this->m_ProPicNum[0] = GRAY_V_NUMDIGIT * 2;
	this->m_ProPicNum[1] = GRAY_H_NUMDIGIT * 2;
	this->m_ProPicNum[2] = PHASE_NUMDIGIT;
	this->m_ProPicNum[3] = PHASE_NUMDIGIT;

	this->m_chessFilePath = "Chess_20160227/";
	this->m_chessFileFrameNum = "0";
	this->m_chessFileCamPath = "";
	this->m_chessFileCamName = "CameraMat";
	
	this->m_chessFileProPath[0] = "Gray_v/";
	this->m_chessFileProPath[1] = "Gray_h/";
	this->m_chessFileProPath[2] = "Phase_v/";
	this->m_chessFileProPath[3] = "Phase_h/";
	this->m_chessFileProName[0] = "GrayMat";
	this->m_chessFileProName[1] = "GrayMat";
	this->m_chessFileProName[2] = "PhaseMat";
	this->m_chessFileProName[3] = "PhaseMat";

	this->m_chessFileSuffix = ".bmp";
}

CSensor::~CSensor()
{

}

// 设置当前标定的帧数。
// 设定之后才能进行读取。默认是从0开始。
bool CSensor::SetChessFrame(int frame)
{
	// 判断参数是否合法
	if ((frame<0) || (frame >= this->m_frameNum))
	{
		ErrorHandling("SetChessFrame->int frame is not valid");
		return false;
	}

	strstream ss;
	ss << frame+1 << '/';
	ss >> this->m_chessFileFrameNum;
	return true;
}

// 获取相机图像。
cv::Mat CSensor::GetCamFrame()
{
	Mat tempMat;
	tempMat = imread(this->m_chessFilePath
		+ this->m_chessFileFrameNum
		+ this->m_chessFileCamPath
		+ this->m_chessFileCamName
		+ this->m_chessFileSuffix, CV_LOAD_IMAGE_GRAYSCALE);
	Mat convertMat;
	tempMat.copyTo(convertMat);
	//resize(tempMat, convertMat, Size(CAMERA_RESLINE, CAMERA_RESROW));
	if (tempMat.empty())
	{
		ErrorHandling("GetCamFrame->imread error.");
	}
	return convertMat;
}

// 获取投影仪图案的总数
int CSensor::GetPicNum(int patternIdx)
{
	if ((patternIdx < 0) || (patternIdx>=4))
	{
		ErrorHandling("GetPicNum->Parameter Error.");
		return false;
	}
	return this->m_ProPicNum[patternIdx];
}

// 获取投影仪图像
cv::Mat CSensor::GetProFrame(int patternIdx, int picIdx)
{
	// 确保参数合法
	if ((patternIdx < 0) || (patternIdx>=4))
	{
		ErrorHandling("CSensor.GetProFrame-> <patternIdx> Parameter Error.");
	}
	if ((picIdx < 0) || (picIdx>=this->m_ProPicNum[patternIdx]))
	{
		ErrorHandling("CSensor.GetProFrame-> <picIdx> Parameter Error.");
	}

	// 读取
	strstream ss;
	string picNum;
	ss << picIdx;
	ss >> picNum;
	Mat tempMat;
	tempMat = imread(this->m_chessFilePath
		+ this->m_chessFileFrameNum
		+ this->m_chessFileProPath[patternIdx]
		+ this->m_chessFileProName[patternIdx]
		+ picNum
		+ this->m_chessFileSuffix, CV_LOAD_IMAGE_GRAYSCALE);
	Mat convertMat;
	tempMat.copyTo(convertMat);
	resize(tempMat, convertMat, Size(CAMERA_RESLINE, CAMERA_RESROW));
	if (tempMat.empty())
	{
		ErrorHandling("GetProFrame->imread error.");
	}
	return convertMat;
}