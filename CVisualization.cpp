#include "head.h"
using namespace std;
using namespace cv;

CVisualization::CVisualization(string winName)
{
	this->m_winName = winName;
	namedWindow(this->m_winName);
}

CVisualization::~CVisualization()
{
	destroyWindow(this->m_winName);
}

int CVisualization::Show(Mat pic, int time, bool norm)
{
	Mat show;
	pic.copyTo(show);
	
	// 需要标准归一化的情况
	if (norm)
	{
		// 确定Mat类别
		int range = 0;
		
		if (show.depth() == CV_8U)
		{
			range = 0xff;
		}
		else if (show.depth() == CV_16U)
		{
			range = 0xffff;
		}

		// 找最大最小值
		int min, max;
		min = range;
		max = 0;
		for (int i = 0; i < show.size().height; i++)
		{
			for (int j = 0; j < show.size().width; j++)
			{
				int value = 0;
				if (show.depth() == CV_8U)
				{
					value = show.at<uchar>(i, j);
				}
				else if (show.depth() == CV_16U)
				{
					value = show.at<ushort>(i, j);
				}
				if (value < min)
					min = value;
				if (value > max)
					max = value;
			}
		}

		// 归一
		show = (show - min) / (max - min) * range;
	}

	imshow(this->m_winName, show);
	return waitKey(time);
}