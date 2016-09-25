#include "CVisualization.h"

CVisualization::CVisualization(string winName)
{
	this->m_winName = winName;
	namedWindow(this->m_winName);
}

CVisualization::~CVisualization()
{
	destroyWindow(this->m_winName);
}

int CVisualization::Show(Mat pic, int time, bool norm, double zoom)
{
	Mat show;
	Size showSize = Size(pic.size().width*zoom, pic.size().height*zoom);
	resize(pic, show, showSize);
	Mat present;
	
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
		else if (show.depth() == CV_64F)
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
				else if (show.depth() == CV_64F)
				{
					value = show.at<double>(i, j);
				}
				if (value < min)
					min = value;
				if (value > max)
					max = value;
			}
		}

		// 归一
		present.create(show.size(), CV_8UC1);
		for (int i = 0; i < show.size().height; i++)
		{
			for (int j = 0; j < show.size().width; j++)
			{
				if (show.depth() == CV_8U)
				{
					present.at<uchar>(i, j) = (show.at<uchar>(i, j) - min) / (max - min) * 0xff;
				}
				else if (show.depth() == CV_16U)
				{
					present.at<uchar>(i, j) = (show.at<ushort>(i, j) - min) / (max - min) * 0xff;
				}
				else if (show.depth() == CV_64F)
				{
					present.at<uchar>(i, j) = (show.at<double>(i, j) - min) / (max - min) * 0xff;
				}
			}
		}
	}
	else
	{
		present = show;
	}

	imshow(this->m_winName, present);
	return waitKey(time);
}