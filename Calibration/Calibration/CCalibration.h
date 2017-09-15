#ifndef _CCALIBRATION_H_
#define _CCALIBRATION_H_

#include <opencv2/opencv.hpp>

#include "CSensor.h"
#include "CDecodeGray.h"
#include "CDecodePhase.h"
#include "CVisualization.h"
#include "GlobalFunction.h"
#include "StaticParameters.h"
using namespace cv;
using namespace std;

// 标定类。用于标定摄像头、投影仪。
// 其中会引用摄像头和投影仪的控制，并创建两个解码器进行解码。
class CCalibration
{
private:
	// 传感器控制类
	CSensor * m_sensor;
	string m_patternPath;
	// 解码器
	CDecodeGray * m_decodeGray;	// 格雷码解码
	CDecodePhase * m_decodePS;		// PS解码
	// 棋盘格的X、Y，以及图片数目
	int m_chessLine;
	int m_chessRow;
	int m_chessNum;
	// 采集到的棋盘格图像和识别结果
	Mat m_chessMat;
	Mat * m_grayV;
	Mat * m_grayH;
	Mat * m_phaseV;
	Mat * m_phaseH;
	Mat * xpro_mats_;
	Mat * ypro_mats_;
	Mat m_chessMatDraw;
	Mat m_proMatDraw;
	//棋盘格顺序编号
	vector<vector<Point2f>> m_camPoint;
	vector<Point2f> m_camPointTmp;
	vector<vector<Point2f>> m_proPoint;
	vector<Point2f> m_proPointTmp;
	vector<vector<Point3f>> m_objPoint;
	vector<Point3f> m_objPointTmp;
	// 标定的矩阵
	Mat m_camMatrix;
	Mat m_camDisCoeffs;
	Mat m_proMatrix;
	Mat m_proDisCoeffs;
	Mat m_R;
	Mat m_T;
	Mat m_E;
	Mat m_F;

	bool ReleaseSpace();			// 释放空间
	bool RecoChessPointObj(int frameIdx);		// 填充m_objPoint
	bool RecoChessPointCam(int frameIdx);		// 填充m_camPoint
	bool RecoChessPointPro(int frameIdx);		// 填充m_proPoint
	bool PushChessPoint(int frameIdx);			// 保存棋盘格，存储到本地
public:
	CCalibration();
	~CCalibration();
	bool Init();				// 初始化
	bool Calibrate();			// 标定主函数。
	bool Result();
};

#endif