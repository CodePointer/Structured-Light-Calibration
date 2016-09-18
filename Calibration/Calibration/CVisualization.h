#ifndef _CVISUALIZATION_H_
#define _CVISUALIZATION_H_

#include <string>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

// 可视化模块，用于debug。自动创建销毁窗口。
class CVisualization
{
private:
	string m_winName;		// 窗口名称
public:
	CVisualization(string winName);
	~CVisualization();
	int Show(Mat pic, int time, bool norm = false, double zoom = 1.0);
};

#endif