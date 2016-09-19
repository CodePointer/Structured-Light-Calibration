#include "CProjector.h"

// 构造函数。初始化一些变量
CProjector::CProjector()
{
	this->m_resRow = PROJECTOR_RESROW;
	this->m_resLine = PROJECTOR_RESLINE;
	this->m_biasLine = PC_BIASLINE;
	this->m_biasRow = PC_BIASROW;
	this->m_winName = "Projector";
}

// 析构函数。暂空。
CProjector::~CProjector()
{

}

// 初始化设备。主要创建一个窗口，并将窗口置于合适位置，进行拉伸。
bool CProjector::InitProjector()
{
	using namespace cv;

	// 创建窗口
	namedWindow(this->m_winName, WINDOW_NORMAL);

	// 调整窗口大小与位置
	setWindowProperty(this->m_winName, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	resizeWindow(this->m_winName, this->m_resLine, this->m_resRow);
	moveWindow(this->m_winName, PC_BIASLINE, PC_BIASROW);

	return true;
}

// 关闭设备。销毁窗口。
bool CProjector::CloseProjector()
{
	using namespace cv;

	destroyWindow(this->m_winName);

	return true;
}

// 放映。使投影仪放映出pic，并使其持续time时间。time单位为ms。
bool CProjector::presentPicture(cv::Mat pic, int time)
{
	using namespace cv;

	// 检查参数是否合法
	/*if (pic.size() != Mat::MSize(this->m_resRow, this->m_resLine))
	{
	std::cout << (pic.size()).width() << std::endl;
	std::cout << pic.size()
	return false;
	}*/

	imshow(this->m_winName, pic);
	waitKey(time);

	return true;
}

bool CProjector::presentPicture(uchar x, int time)
{
	using namespace cv;

	Mat pic;
	pic.create(this->m_resRow, this->m_resLine, CV_8UC1);
	pic.setTo(x);

	imshow(this->m_winName, pic);
	waitKey(time);

	return true;
}