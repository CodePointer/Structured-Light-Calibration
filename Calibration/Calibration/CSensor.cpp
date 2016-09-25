#include "CSensor.h"


// 传感器模块。
// 用于模拟传感器传入数据。
// 由于标定内容之前已经离线采集好，因此这个类只需要从文件中读入文件即可。

CSensor::CSensor()
{
	this->m_patterns = NULL;
	this->m_patternNum = 0;
	this->m_nowNum = 0;
	this->m_filePath = "";
	this->m_fileName = "";
	this->m_fileSuffix = "";

	this->m_camera = NULL;
	this->m_projector = NULL;
}

CSensor::~CSensor()
{
	if (this->m_patterns != NULL)
	{
		delete[]this->m_patterns;
		this->m_patterns = NULL;
	}
	if (this->m_camera != NULL)
	{
		delete[]this->m_camera;
		this->m_camera = NULL;
	}
	if (this->m_projector != NULL)
	{
		delete[]this->m_projector;
		this->m_projector = NULL;
	}
}

// 初始化传感器
// 创建Camera、Projector并进行初始化
bool CSensor::InitSensor()
{
	bool status;

	this->m_camera = new CCamera();
	status = this->m_camera->InitCamera();

	this->m_projector = new CProjector();
	status = this->m_projector->InitProjector();

	return status;
}

// 关闭传感器
bool CSensor::CloseSensor()
{
	bool status = true;

	if (this->m_camera != NULL)
	{
		this->m_camera->CloseCamera();
		delete[]this->m_camera;
		this->m_camera = NULL;
	}
	if (this->m_projector != NULL)
	{
		delete[]this->m_projector;
		this->m_projector = NULL;
	}
	this->UnloadPatterns();

	return status;
}

// 读取图案
bool CSensor::LoadPatterns(int patternNum, string filePath, string fileName, string fileSuffix)
{
	// 检查状态是否合法
	if (this->m_patterns != NULL)
	{
		this->UnloadPatterns();
	}

	// 设置参数，申请空间
	this->m_patternNum = patternNum;
	this->m_nowNum = 0;
	this->m_filePath = filePath;
	this->m_fileName = fileName;
	this->m_fileSuffix = fileSuffix;
	this->m_patterns = new Mat[this->m_patternNum];

	// 读取
	for (int i = 0; i < patternNum; i++)
	{
		Mat tempMat;
		string idx2Str;
		strstream ss;
		ss << i ;
		ss >> idx2Str;
		string temp = this->m_filePath
			+ this->m_fileName
			+ idx2Str
			+ this->m_fileSuffix;
		tempMat = imread(temp, CV_LOAD_IMAGE_GRAYSCALE);
		tempMat.copyTo(this->m_patterns[i]);

		if (tempMat.empty())
		{
			ErrorHandling("CSensor::LoadPatterns::<Read>, imread error, idx=" + idx2Str);
		}
	}

	return true;
}

// 释放已读取图案
bool CSensor::UnloadPatterns()
{
	if (this->m_patterns != NULL)
	{
		delete[]this->m_patterns;
		this->m_patterns = NULL;
	}
	this->m_patternNum = 0;
	this->m_nowNum = 0;
	this->m_filePath = "";
	this->m_fileName = "";
	this->m_fileSuffix = "";

	return true;
}

// 设置投影仪投影的图像
bool CSensor::SetProPicture(int nowNum)
{
	bool status = true;

	// 检查参数是否合法
	if (nowNum >= this->m_patternNum)
	{
		status = false;
		return status;
	}

	this->m_nowNum = nowNum;

	status = this->m_projector->presentPicture(
		this->m_patterns[this->m_nowNum], 200);

	return status;
}

// 获取相机图像
Mat CSensor::GetCamPicture()
{
	bool status = true;

	Mat tempMat;
	status = this->m_camera->getPicture(tempMat);

	return tempMat;
}

// 获取投影仪投影的图像
Mat CSensor::GetProPicture()
{
	return this->m_patterns[this->m_nowNum];
}