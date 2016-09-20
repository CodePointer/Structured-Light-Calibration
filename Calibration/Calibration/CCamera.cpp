#include "CCamera.h"

// 构造函数：
// 设置一些基础参数。主要为private里面的静态参数
CCamera::CCamera()
{
	this->sumDeviceNum = 0;
	this->nowDeviceNum = 0;
	this->resLine = 0;
	this->resRow = 0;
	this->BufferNum = 1;
	this->ppBuffer = NULL;
}

// 析构函数：
// 保证摄像机已经断开。
CCamera::~CCamera()
{
	if (this->nowDeviceNum != 0)
		EndHVDevice(this->DeviceHandle);
	if (this->ppBuffer != NULL)
	{
		for (int i = 1; i < BufferNum; i++)
			if (ppBuffer[i] != NULL)
			{
				delete[](ppBuffer[i]);
				ppBuffer[i] = NULL;
			}
		delete[]ppBuffer;
		ppBuffer = NULL;
	}
}

// 获取总共摄像头数目。初始值为0。
int CCamera::getSumDeviceNum()
{
	return this->sumDeviceNum;
}

// 获取当前采集的摄像头序号。为0，则无使用中的摄像头。
int CCamera::getNowDeviceNum()
{
	return this->nowDeviceNum;
}

// 初始化设备以备采集。
// 包括打开摄像机，获取相关信息并填充。
bool CCamera::InitCamera()
{
	HVSTATUS status;

	this->resRow = CAMERA_RESROW;
	this->resLine = CAMERA_RESLINE;

	// 获取设备总数
	int sumNum;
	status = HVGetDeviceTotal(&sumNum);
	HV_VERIFY(status);
	this->sumDeviceNum = sumNum;

	// 打开1号摄像机
	if (this->sumDeviceNum < 1)
		return false;
	status = BeginHVDevice(1, &(this->DeviceHandle));
	HV_VERIFY(status);
	this->nowDeviceNum = 1;

	// 设置1号摄像机格式
	status = HVSetResolution(this->DeviceHandle, RES_MODE1);
	HV_VERIFY(status);
	HVSetOutputWindow(DeviceHandle, 0, 0, this->resLine, this->resRow);
	HV_VERIFY(status);

	// 申请缓冲区。
	this->ppBuffer = new BYTE*[this->BufferNum];
	for (int i = 0; i < this->BufferNum; i++)
	{
		ppBuffer[i] = new BYTE[this->resLine*this->resRow];
	}
	this->used = this->BufferNum;

	// 获取部分图像保证稳定
	int stableNum = 5;
	Mat tempMat;
	while (stableNum-- != 0)
	{
		this->getPicture(tempMat);
	}

	return true;
}

// 从摄像机获取一组图像
bool CCamera::SnapShot()
{
	HVSTATUS status;
	int maxAttempt = 30;
	int attempt = 0;

	// 获取图片
	attempt = 0;
	status = HVSnapShot(this->DeviceHandle, this->ppBuffer, this->BufferNum);
	while ((status != STATUS_OK) && (attempt < maxAttempt))
	{
		attempt++;
		status = HVSnapShot(this->DeviceHandle, this->ppBuffer, this->BufferNum);
	}

	// 判断是否获取到图像
	if (attempt >= maxAttempt)
	{
		std::cout << HVGetErrorString(status) << std::endl;
		return false;
	}

	this->used = 0;
	return true;
}

// 将图像转换为Mat格式
bool CCamera::getPicture(cv::Mat & pic)
{
	// 查看缓冲区是否有剩余图片
	if (this->used >= this->BufferNum)
	{
		if (!this->SnapShot())
			return false;
	}

	// 创建Mat
	pic =Mat(Size(this->resLine, this->resRow), CV_8UC1, this->ppBuffer[this->used]);
	this->used++;	// 更新缓冲区信息

	return true;
}

// 关闭摄像头终止采集。
bool CCamera::CloseCamera()
{
	HVSTATUS status;

	// 关闭摄像机。
	status = EndHVDevice(this->DeviceHandle);
	HV_VERIFY(status);

	// 清除缓冲区
	for (int i = 0; i < BufferNum; i++)
		if (ppBuffer[i] != NULL)
		{
			delete[](ppBuffer[i]);
			ppBuffer[i] = NULL;
		}
	delete[]ppBuffer;
	ppBuffer = NULL;

	return true;
}