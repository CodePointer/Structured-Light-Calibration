#include "head.h"

using namespace cv;
using namespace std;

int main()
{
	// 创建类：
	CCalibration myCalibration;		// 标定类
	myCalibration.Init();
	myCalibration.Calibrate();
	myCalibration.Result();
	system("PAUSE");
	return 0;
}