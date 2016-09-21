#include "CCalibration.h"

CVisualization myCamera("Camera");
CVisualization myDebug("Debug");
CVisualization myProjector("Projector");

// 构造函数。对一些参数赋初值
CCalibration::CCalibration()
{
	this->m_sensor = NULL;
	this->m_decodeGray = NULL;
	this->m_decodePS = NULL;
	this->m_grayV = NULL;
	this->m_grayH = NULL;
	this->m_phaseV = NULL;
	this->m_phaseH = NULL;
}

// 析构函数。确保所有空间已经释放。
CCalibration::~CCalibration()
{
	this->ReleaseSpace();
}

// 释放空间。安全的释放指针的空间。
bool CCalibration::ReleaseSpace()
{
	if (this->m_sensor != NULL)
	{
		delete(this->m_sensor);
		this->m_sensor = NULL;
	}
	if (this->m_decodeGray != NULL)
	{
		delete[](this->m_decodeGray);
		this->m_decodeGray = NULL;
	}
	if (this->m_decodePS != NULL)
	{
		delete[](this->m_decodePS);
		this->m_decodePS = NULL;
	}
	if (this->m_grayV != NULL)
	{
		delete[](this->m_grayV);
		this->m_grayV = NULL;
	}
	if (this->m_grayH != NULL)
	{
		delete[](this->m_grayH);
		this->m_grayH = NULL;
	}
	if (this->m_phaseV != NULL)
	{
		delete[](this->m_phaseV);
		this->m_phaseV = NULL;
	}
	if (this->m_phaseH!= NULL)
	{
		delete[](this->m_phaseH);
		this->m_phaseH = NULL;
	}

	return true;
}

// 初始化
bool CCalibration::Init()
{
	if ((this->m_sensor != NULL))
		return false;
	if ((this->m_decodeGray != NULL) || (this->m_decodePS != NULL))
		return false;

	this->m_sensor = new CSensor;
	this->m_sensor->InitSensor();
	this->m_patternPath = "Patterns/";

	this->m_decodeGray = new CDecodeGray;
	this->m_decodePS = new CDecodePhase;
	this->m_chessLine = CHESS_LINE;
	this->m_chessRow = CHESS_ROW;
	this->m_chessNum = CHESS_FRAME_NUMBER;

	this->m_grayV = new Mat[GRAY_V_NUMDIGIT * 2];
	this->m_grayH = new Mat[GRAY_H_NUMDIGIT * 2];
	this->m_phaseV = new Mat[PHASE_NUMDIGIT];
	this->m_phaseH = new Mat[PHASE_NUMDIGIT];

	return true;
}

// 标定主函数
bool CCalibration::Calibrate()
{
	bool status = true;

	// 判断参数是否合法
	if ((this->m_sensor == NULL))
		return false;
	if ((this->m_decodeGray == NULL) || (this->m_decodePS == NULL))
		return false;

	// 初始化：清空三个Vector
	std::vector<std::vector<cv::Point2f>>().swap(this->m_camPoint);
	std::vector<std::vector<cv::Point2f>>().swap(this->m_proPoint);
	std::vector<std::vector<cv::Point3f>>().swap(this->m_objPoint);
	//CVisualization visual("Result", 100);

	// 识别棋盘格，即填充上述三个Vector
	for (int i = 0; i < this->m_chessNum; i++)
	{
		// 填充ObjPoint；
		status = this->RecoChessPointObj(i);
		printf("For %dth picture: ObjPoint finished.\n", i+1);

		// 填充CamPoint；
		status = this->RecoChessPointCam(i);
		printf("For %dth picture: CamPoint finished.\n", i+1);

		// 填充ProPoint；
		status = this->RecoChessPointPro(i);
		printf("For %dth picture: ProPoint finished.\n", i+1);

		// 判断识别结果是否正确，如果正确则存储角点信息
		int key = 0;
		key = myCamera.Show(this->m_chessMatDraw, 100, false, 0.5);
		key = myProjector.Show(this->m_proMatDraw, 0, false, 0.5);
		if (key == 'c')
		{
			status = this->PushChessPoint(i);
		}
		else
		{
			i = i - 1;
			continue;
		}

		//visual.Show(this->m_chessMat);
		printf("Finish %dth picture.\n", i+1);
	}

	// 标定部分
	printf("Begin Calibrating.\n");
	calibrateCamera(this->m_objPoint,
		this->m_camPoint,
		Size(CAMERA_RESLINE, CAMERA_RESROW),
		this->m_camMatrix,
		this->m_camDisCoeffs,
		noArray(),
		noArray(),
		CALIB_FIX_K3 + CALIB_FIX_PRINCIPAL_POINT,
		TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 300, 1e-16));
	printf("Finish Camera.\n");
	calibrateCamera(this->m_objPoint,
		this->m_proPoint,
		Size(PROJECTOR_RESLINE, PROJECTOR_RESROW),
		this->m_proMatrix,
		this->m_proDisCoeffs,
		noArray(),
		noArray(),
		CALIB_FIX_K3,
		TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 300, 1e-16));
	printf("Finish Projector.\n");
	stereoCalibrate(this->m_objPoint,
		this->m_camPoint,
		this->m_proPoint,
		this->m_camMatrix,
		this->m_camDisCoeffs,
		this->m_proMatrix,
		this->m_proDisCoeffs,
		Size(CAMERA_RESLINE, CAMERA_RESROW),
		this->m_R,
		this->m_T,
		this->m_E,
		this->m_F,
		TermCriteria(TermCriteria::COUNT + TermCriteria::EPS, 300, 1e-16));
	printf("Finish stereo Calibration.\n");

	return true;
}

// 输出保存数据
bool CCalibration::Result()
{
	FileStorage fs("Result.yml", FileStorage::WRITE);
	cout << "CamMat" << endl;
	fs << "CamMat" << this->m_camMatrix;
	cout << this->m_camMatrix << endl;
	
	cout << "ProMat" << endl;
	fs << "ProMat" << this->m_proMatrix;
	cout << this->m_proMatrix << endl;

	cout << "R,T" << endl;
	fs << "R" << this->m_R;
	fs << "T" << this->m_T;
	cout << this->m_R << endl;
	cout << this->m_T << endl;
	fs.release();
	return true;
}

// 识别Obj中的棋盘格并填充
bool CCalibration::RecoChessPointObj(int frameIdx)
{
	vector<Point3f>().swap(this->m_objPointTmp);
	// 填充三维棋盘格坐标
	for (int i = 0; i < this->m_chessLine; i++)
	{
		for (int j = 0; j < this->m_chessRow; j++)
		{
			this->m_objPointTmp.push_back(Point3f(i, j, 0));
		}
	}
	
	return true;
}

// 识别Cam中的棋盘格并填充
bool CCalibration::RecoChessPointCam(int frameIdx)
{
	bool status = true;
	
	// 清空原来的角点信息
	vector<Point2f>().swap(this->m_camPointTmp);

	// 设定投影仪投影图案
	this->m_sensor->LoadPatterns(1, this->m_patternPath, "empty", ".bmp");
	this->m_sensor->SetProPicture(0);

	// 尝试识别角点
	while (true)
	{
		// 获取当前相机帧
		Mat camMat;
		camMat = this->m_sensor->GetCamPicture();
		camMat.copyTo(this->m_chessMat);
		camMat.copyTo(this->m_chessMatDraw);

		// 寻找棋盘格
		int maxAttempt = 10;
		int k = 0;
		int found = 0;
		while (k++ < maxAttempt)
		{
			found = findChessboardCorners(this->m_chessMat, Size(this->m_chessRow, this->m_chessLine), this->m_camPointTmp, CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE);
			cv::drawChessboardCorners(this->m_chessMatDraw, Size(this->m_chessRow, this->m_chessLine), this->m_camPointTmp, found);
			if (found)
			{
				break;
			}
		}

		// 如果成功寻找，则跳出。
		if (k < maxAttempt)
		{
			cornerSubPix(camMat,
				this->m_camPointTmp,
				Size(5, 5),
				Size(-1, -1),
				TermCriteria(TermCriteria::MAX_ITER + TermCriteria::EPS,
				30,
				0.1));
			break;
		}
	}

	return status;
}

// 识别Pro中的棋盘格并填充
bool CCalibration::RecoChessPointPro(int frameIdx)
{
	bool status = true;

	// 清空原来的角点信息
	vector<Point2f>().swap(this->m_proPointTmp);

	// 根据结构光解码相机视野中每个点的投影仪坐标
	Mat tempMat;
	Mat vGrayMat;
	Mat hGrayMat;
	Mat vPhaseMat;
	Mat hPhaseMat;
	Mat vProjectorMat;
	Mat hProjectorMat;

	// vGray
	this->m_sensor->LoadPatterns(GRAY_V_NUMDIGIT, this->m_patternPath, "vGray", ".bmp");
	this->m_decodeGray->SetNumDigit(GRAY_V_NUMDIGIT, true);
	this->m_decodeGray->SetMatFileName(this->m_patternPath, "vGrayCode.txt");
	for (int i = 0; i < GRAY_V_NUMDIGIT * 2; i++)		// Projector
	{
		this->m_sensor->SetProPicture(i);
		tempMat = this->m_sensor->GetCamPicture();
		this->m_decodeGray->SetMat(i, tempMat);
	}
	this->m_sensor->UnloadPatterns();
	this->m_decodeGray->Decode();
	vGrayMat = this->m_decodeGray->GetResult();

	// hGray
	this->m_sensor->LoadPatterns(GRAY_H_NUMDIGIT, this->m_patternPath, "hGray", ".bmp");
	this->m_decodeGray->SetNumDigit(GRAY_H_NUMDIGIT, false);
	this->m_decodeGray->SetMatFileName(this->m_patternPath, "vGrayCode.txt");
	for (int i = 0; i < GRAY_H_NUMDIGIT * 2; i++)		// Projector
	{
		this->m_sensor->SetProPicture(i);
		tempMat = this->m_sensor->GetCamPicture();
		this->m_decodeGray->SetMat(i, tempMat);
	}
	this->m_sensor->UnloadPatterns();
	this->m_decodeGray->Decode();
	hGrayMat = this->m_decodeGray->GetResult();

	// vPhase
	this->m_sensor->LoadPatterns(PHASE_NUMDIGIT, this->m_patternPath, "vPhase", ".bmp");
	int v_pixPeriod = PROJECTOR_RESLINE / (1 << (GRAY_V_NUMDIGIT - 1));
	this->m_decodePS->SetNumMat(PHASE_NUMDIGIT, v_pixPeriod);
	for (int i = 0; i < PHASE_NUMDIGIT; i++)
	{
		this->m_sensor->SetProPicture(i);
		tempMat = this->m_sensor->GetCamPicture();
		this->m_decodePS->SetMat(i, tempMat);
	}
	this->m_sensor->UnloadPatterns();
	this->m_decodePS->Decode();
	vPhaseMat = this->m_decodePS->GetResult();

	// hPhase
	this->m_sensor->LoadPatterns(PHASE_NUMDIGIT, this->m_patternPath, "vPhase", ".bmp");
	int h_pixPeriod = PROJECTOR_RESROW / (1 << (GRAY_H_NUMDIGIT - 1));
	this->m_decodePS->SetNumMat(PHASE_NUMDIGIT, h_pixPeriod);
	for (int i = 0; i < PHASE_NUMDIGIT; i++)
	{
		this->m_sensor->SetProPicture(i);
		tempMat = this->m_sensor->GetCamPicture();
		this->m_decodePS->SetMat(i, tempMat);
	}
	this->m_sensor->UnloadPatterns();
	this->m_decodePS->Decode();
	hPhaseMat = this->m_decodePS->GetResult();

	// 合并
	int vGrayNum = 1 << GRAY_V_NUMDIGIT;
	int vGrayPeriod = PROJECTOR_RESLINE / vGrayNum;
	for (int h = 0; h < CAMERA_RESROW; h++)
	{
		for (int w = 0; w < CAMERA_RESLINE; w++)
		{
			double grayVal = vGrayMat.at<double>(h, w);
			double phaseVal = vPhaseMat.at<double>(h, w);
			if ((int)(grayVal / 20) % 2 == 0)
			{
				if (phaseVal > (double)v_pixPeriod * 0.75)
				{
					vPhaseMat.at<double>(h, w) = phaseVal - v_pixPeriod;
				}
			}
			else
			{
				if (phaseVal < (double)v_pixPeriod * 0.25)
				{
					vPhaseMat.at<double>(h, w) = phaseVal + v_pixPeriod;
				}
			}
		}
	}
	vProjectorMat = vGrayMat + vPhaseMat;
	int hGrayNum = 1 << GRAY_H_NUMDIGIT;
	int hGrayPeriod = PROJECTOR_RESLINE / hGrayNum;
	for (int h = 0; h < CAMERA_RESROW; h++)
	{
		for (int w = 0; w < CAMERA_RESLINE; w++)
		{
			double grayVal = hGrayMat.at<double>(h, w);
			double phaseVal = hPhaseMat.at<double>(h, w);
			if ((int)(grayVal / 20) % 2 == 0)
			{
				if (phaseVal >(double)v_pixPeriod * 0.75)
				{
					hPhaseMat.at<double>(h, w) = phaseVal - v_pixPeriod;
				}
			}
			else
			{
				if (phaseVal < (double)v_pixPeriod * 0.25)
				{
					hPhaseMat.at<double>(h, w) = phaseVal + v_pixPeriod;
				}
			}
		}
	}
	hProjectorMat = hGrayMat + hPhaseMat;

	myDebug.Show(vProjectorMat, 00, true, 0.5);
	myDebug.Show(hProjectorMat, 00, true, 0.5);

	// 将相机格点坐标转换为投影仪格点坐标
	vector<Point2f>::iterator i;
	for (i = this->m_camPointTmp.begin(); i != this->m_camPointTmp.end(); ++i)
	{
		Point2f cam = *i;
		Point2f pro;
		int X = cam.x;
		int Y = cam.y;
		pro.x = vProjectorMat.at<ushort>(Y, X);
		pro.y = hProjectorMat.at<ushort>(Y, X);
		this->m_proPointTmp.push_back(pro);
		//cout << "(" << X << "," << Y << ") -> (" << pro.x << "," << pro.y << ")" << endl;
	}

	// 绘制投影仪转化后的格点坐标
	this->m_proMatDraw.create(800, 1280, CV_8UC1);
	this->m_proMatDraw.setTo(0);
	drawChessboardCorners(this->m_proMatDraw, Size(this->m_chessRow, this->m_chessLine), this->m_proPointTmp, true);

	return status;
}

// 保存棋盘格，存储到本地
bool CCalibration::PushChessPoint(int frameIdx)
{
	bool status = true;

	// 保存棋盘格
	this->m_objPoint.push_back(this->m_objPointTmp);
	this->m_camPoint.push_back(this->m_camPointTmp);
	this->m_proPoint.push_back(this->m_proPointTmp);

	// 存储本地图像
	strstream ss;
	string IdxtoStr;
	ss << frameIdx;
	ss >> IdxtoStr;

	// 相机图像
	CStorage camChessMat;
	camChessMat.SetMatFileName("RecoChessPoint/" + IdxtoStr + "/", "CameraMat", ".bmp");
	camChessMat.Store(&(this->m_chessMat), 1);
	camChessMat.SetMatFileName("RecoChessPoint/" + IdxtoStr + "/", "CameraMatDraw", ".bmp");
	camChessMat.Store(&(this->m_chessMatDraw), 1);

	// 投影仪图像
	CStorage proChessMat;
	proChessMat.SetMatFileName("RecoChessPoint/" + IdxtoStr + "/", "vGray", ".bmp");
	proChessMat.Store(this->m_grayV, GRAY_V_NUMDIGIT * 2);
	proChessMat.SetMatFileName("RecoChessPoint/" + IdxtoStr + "/", "hGray", ".bmp");
	proChessMat.Store(this->m_grayV, GRAY_H_NUMDIGIT * 2);
	proChessMat.SetMatFileName("RecoChessPoint/" + IdxtoStr + "/", "vPhase", ".bmp");
	proChessMat.Store(this->m_grayV, PHASE_NUMDIGIT);
	proChessMat.SetMatFileName("RecoChessPoint/" + IdxtoStr + "/", "hPhase", ".bmp");
	proChessMat.Store(this->m_grayV, PHASE_NUMDIGIT);
	proChessMat.SetMatFileName("RecoChessPoint/" + IdxtoStr + "/", "ProMatDraw", ".bmp");
	proChessMat.Store(&(this->m_proMatDraw), 1);

	return status;
}