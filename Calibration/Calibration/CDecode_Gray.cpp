#include "head.h"
#include <fstream>
#include <strstream>
using namespace cv;

// 构造函数。设定一些默认设置。
CDecode_Gray::CDecode_Gray()
{
	this->m_numDigit = 0;
	this->m_grayCodeSize = 0;
	this->m_gray2bin = NULL;
	this->m_codeFileName = "";
	this->resRow = CAMERA_RESROW;
	this->resLine = CAMERA_RESLINE;
	this->m_grePicture = NULL;
	this->m_binPicture = NULL;
	this->m_visual = NULL;
}

// 析构函数。确保释放空间。
CDecode_Gray::~CDecode_Gray()
{
	this->DeleteSpace();
}

// 输入相应的相机视图
bool CDecode_Gray::SetCamMat(cv::Mat pic)
{
	pic.copyTo(this->m_camPicture);
	return true;
}

// 输入相应灰度图
bool CDecode_Gray::SetMat(int num, cv::Mat pic)
{
	if (this->m_grePicture == NULL)
	{
		ErrorHandling("CDecode_Gray.SetMat->grePicture Space is not allocated.");
		return false;
	}
	pic.copyTo(this->m_grePicture[num]);
	return true;
}

// 设置格雷码位数
bool CDecode_Gray::SetNumDigit(int numDigit, bool ver)
{
	// 首先判断参数是否合法
	if ((numDigit <= 0) || (numDigit > 16))
		return false;
	
	// 传参
	this->m_numDigit = numDigit;
	this->m_grayCodeSize = 1 << (this->m_numDigit);
	this->m_vertical = ver;

	// 分配相应空间
	if ((this->m_gray2bin != NULL) || (this->m_grePicture != NULL) || (this->m_binPicture != NULL))
		this->DeleteSpace();
	this->AllocateSpace();

	return true;
}

// 设置存储格雷码的文件名
bool CDecode_Gray::SetMatFileName(std::string codeFilePath,
	std::string codeFileName)
{
	this->m_codeFilePath = codeFilePath;
	this->m_codeFileName = codeFileName;
	return true;
}

// 为输入的矩阵、short类型内容申请空间
bool CDecode_Gray::AllocateSpace()
{
	using namespace cv;

	// 判断参数是否合法
	if ((this->m_numDigit <= 0) || (this->m_numDigit > 16))
		return false;

	// 申请空间并初始化
	this->m_gray2bin = new short[this->m_grayCodeSize];
	this->m_grePicture = new Mat[this->m_grayCodeSize];
	this->m_binPicture = new Mat[this->m_grayCodeSize];

	return true;
}

// 删除所有空间
bool CDecode_Gray::DeleteSpace()
{
	if (this->m_gray2bin != NULL)
	{
		delete[](this->m_gray2bin);
		this->m_gray2bin = NULL;
	}
	if (this->m_grePicture != NULL)
	{
		delete[](this->m_grePicture);
		this->m_grePicture = NULL;
	}
	if (this->m_binPicture != NULL)
	{
		delete[](this->m_binPicture);
		this->m_binPicture = NULL;
	}
	if (this->m_visual != NULL)
	{
		delete(this->m_visual);
		this->m_visual = NULL;
	}
	return true;
}

// 解码。
bool CDecode_Gray::Decode()
{
	using namespace cv;

	// 读入解码文件
	std::ifstream codeFile;
	codeFile.open(this->m_codeFilePath + this->m_codeFileName, std::ios::in);
	if (!codeFile)
	{
		ErrorHandling("Gray Decode->Open file error.");
		return false;
	}
	for (int i = 0; i < this->m_grayCodeSize; i++)
	{
		int binCode, grayCode;
		codeFile >> binCode >> grayCode;
		this->m_gray2bin[grayCode] = binCode;
	}

	// 分别处理所有的灰度图，将其转换为二值内容
	if (!this->Grey2Bin())
		return false;

	// 根据二值图恢复坐标
	if (!this->CountResult())
		return false;

	// 可视化部分
	this->Visualize();

	return true;
}

// 获取解码结果
cv::Mat CDecode_Gray::GetResult()
{
	Mat result;
	this->m_result.copyTo(result);
	return result;
}

// 将灰度图加工为二值图，以便进一步处理
bool CDecode_Gray::Grey2Bin()
{
	using namespace cv;

	// 填充每一张binPicture
	for (int binIdx = 0; binIdx < this->m_numDigit; binIdx++)
	{
		// 获取两张图片的差
		Mat tempMat;
		tempMat = this->m_grePicture[binIdx * 2] - this->m_grePicture[binIdx * 2 + 1];
		
		// 根据相减值进行二值化
		this->m_binPicture[binIdx].create(Size(this->resLine, this->resRow), CV_8UC1);
		for (int i = 0; i < this->resRow; i++)
		{
			for (int j = 0; j < this->resLine; j++)
			{
				uchar value = tempMat.at<uchar>(i, j);
				if (tempMat.at<uchar>(i, j) > 0)
					this->m_binPicture[binIdx].at<uchar>(i, j) = 0xFF;
				else
					this->m_binPicture[binIdx].at<uchar>(i, j) = 0;
			}
		}
	}
	return true;
}

// 根据二值图统计结果
bool CDecode_Gray::CountResult()
{
	int pixPeriod = 0;
	if (this->m_vertical)
		pixPeriod = PROJECTOR_RESLINE / this->m_grayCodeSize;
	else
		pixPeriod = PROJECTOR_RESROW / this->m_grayCodeSize;

	m_result.create(resRow, resLine, CV_16UC1);
	for (int i = 0; i < this->resRow; i++)
	{
		for (int j = 0; j < this->resLine; j++)
		{
			short grayCode = 0;
			for (int binIdx = 0; binIdx < this->m_numDigit; binIdx++)
			{
				if (this->m_binPicture[binIdx].at<uchar>(i, j) == 255)
				{
					grayCode += 1 << binIdx;
				}
			}
			this->m_result.at<short>(i, j) = this->m_gray2bin[grayCode] * pixPeriod;
		}
	}
	return true;
}

// 在计算机上显示图片
bool CDecode_Gray::Visualize()
{
	if (VISUAL_DEBUG)
	{
		// 如果类不存在，则创建一个
		if (this->m_visual == NULL)
		{
			this->m_visual = new CVisualization("GrayDecoder");
		}

		// 显示中间的二值化图像
		for (int i = 0; i < this->m_numDigit; i++)
		{
			this->m_visual->Show(this->m_binPicture[i], 300);
		}

		// 显示最终解码结果（归一化）
		this->m_visual->Show(this->m_result, 1000, true);
	}
	return true;
}