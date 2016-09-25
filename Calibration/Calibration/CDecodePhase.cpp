#include "CDecodePhase.h"

CDecodePhase::CDecodePhase()
{
	this->m_numMat = 0;
	this->m_pixPeroid = 16;
	this->m_resRow = CAMERA_RESROW;
	this->m_resLine = CAMERA_RESLINE;
	this->m_grePicture = NULL;
	this->m_visual = NULL;
}

CDecodePhase::~CDecodePhase()
{
	this->DeleteSpace();
}

// 为输入的矩阵申请空间
bool CDecodePhase::AllocateSpace()
{
	// 判断参数是否合法
	if (this->m_numMat <= 0)
		return false;

	// 申请空间并初始化
	this->m_grePicture = new Mat[this->m_numMat];

	return true;
}

// 删除所有空间
bool CDecodePhase::DeleteSpace()
{
	if (this->m_grePicture != NULL)
	{
		delete[](this->m_grePicture);
		this->m_grePicture = NULL;
	}
	if (this->m_visual != NULL)
	{
		delete(this->m_visual);
		this->m_visual = NULL;
	}
	return true;
}

// 根据灰度图统计结果
bool CDecodePhase::CountResult()
{
	this->m_result.create(this->m_resRow, this->m_resLine, CV_64FC1);
	Mat SinValue0;
	Mat SinValue1;

	for (int i = 0; i < this->m_resRow; i++)
	{
		for (int j = 0; j < this->m_resLine; j++)
		{
			// 先获取该点灰度值
			float greyValue0 = this->m_grePicture[0].at<uchar>(i, j);	// greyValue0 = (sin(x) + 1) * 127;
			float greyValue1 = this->m_grePicture[1].at<uchar>(i, j);	// greyValue1 = (sin(x + CV_PI / 2) + 1) * 127
			float greyValue2 = this->m_grePicture[2].at<uchar>(i, j);	// greyValue2 = (sin(x + CV_PI) + 1) * 127
			float greyValue3 = this->m_grePicture[3].at<uchar>(i, j);	// greyValue3 = (sin(x + 3 * CV_PI / 2) + 1) * 127
			// 将灰度值转变为sin值
			float sinValue = (greyValue0 - greyValue2) / 2;
			float cosValue = (greyValue1 - greyValue3) / 2;
			// 计算x
			float x = cvFastArctan(sinValue, cosValue);			
			// 计算偏移量
			float pix = (x) / (360)*(double)(this->m_pixPeroid);
			pix += 0.5;
			if (pix > this->m_pixPeroid)
			{
				pix -= this->m_pixPeroid;
			}
			this->m_result.at<double>(i, j) = (double)pix;
		}
	}

	return true;
}

// 解码
bool CDecodePhase::Decode()
{
	// 根据灰度图恢复坐标
	if (!this->CountResult())
	{
		ErrorHandling("CDecodePhase.Decode()->CountResult fault");
		return false;
	}

	// 可视化
	this->Visualize();

	return true;
}

// 获取解码结果
Mat CDecodePhase::GetResult()
{
	Mat result;
	this->m_result.copyTo(result);
	return result;
}

// 输入相应灰度图
bool CDecodePhase::SetMat(int num, cv::Mat pic)
{
	if (this->m_grePicture == NULL)
	{
		ErrorHandling("CDecodePhase.SetMat->grePicture Space is not allocated.");
		return false;
	}
	pic.copyTo(this->m_grePicture[num]);
	return true;
}

// 设置参数
bool CDecodePhase::SetNumMat(int numMat, int pixperiod)
{
	// 判断参数是否合法
	if ((numMat <= 0))
		return false;

	// 传参
	this->m_numMat = numMat;
	this->m_pixPeroid = pixperiod;

	// 分配空间
	if ((this->m_grePicture != NULL))
	{
		this->DeleteSpace();
	}
	this->AllocateSpace();

	return true;
}

// 在计算机上显示图片
bool CDecodePhase::Visualize()
{
	if (VISUAL_DEBUG)
	{
		// 如果类不存在，则创建一个
		if (this->m_visual == NULL)
		{
			this->m_visual = new CVisualization("PhaseDecoder");
		}

		// 显示灰度图像
		for (int i = 0; i < this->m_numMat; i++)
		{
			this->m_visual->Show(this->m_grePicture[i], 300);
		}

		// 显示最终解码结果（归一化）
		this->m_visual->Show(this->m_result, 1000, true);
	}
	return true;
}