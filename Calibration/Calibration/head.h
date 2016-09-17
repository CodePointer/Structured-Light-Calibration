// OpenCV
#include <opencv2/opencv.hpp>

// 其他常用库
#include <vector>
#include <direct.h>

#define VISUAL_DEBUG false
#define CHESS_FRAME_NUMBER 15

// 一些常量
static int PROJECTOR_RESLINE = 1280;
static int PROJECTOR_RESROW = 800;
static int CAMERA_RESLINE = 1280;	// 1280
static int CAMERA_RESROW = 1024;	// 1024
static int PC_BIASLINE = 1366;
static int PC_BIASROW = 0;
static int GRAY_V_NUMDIGIT = 6;
static int GRAY_H_NUMDIGIT = 5;
static int PHASE_NUMDIGIT = 4;
static int SHOW_PICTURE_TIME = 500;		// 默认Visual图片的持续时间

// 错误处理函数
int ErrorHandling(std::string message);

// 传感器模块。模拟
class CSensor
{
private:
	int m_frameNum;					// 标定的棋盘格帧数。
	int m_ProPicNum[4];				// 各部分Pattern的总计图案数

	std::string m_chessFilePath;	// 存储文件的总路径
	std::string m_chessFileFrameNum;	// 当前标定的帧数
	std::string m_chessFileCamPath;	// 用于相机标定的文件路径
	std::string m_chessFileCamName;	// 用于相机标定的文件名
	std::string m_chessFileProPath[4];	// 用于投影仪标定的文件路径
	std::string m_chessFileProName[4];	// 用于投影仪标定的文件名
	std::string m_chessFileSuffix;	// 文件后缀名

public:
	CSensor();
	~CSensor();
	bool SetChessFrame(int frame);	// 设定当前标定的帧数	
	cv::Mat GetCamFrame();			// 获取相机图像
	int GetPicNum(int patternIdx);	// 获取投影仪图案的总数
	cv::Mat GetProFrame(int patternIdx, int picIdx);	// 获取投影仪图像
};

// 数据存储模块。存储中间数据做存档
class CStorage
{
private:
	std::string m_matFilePath;
	std::string m_matFileName;
	std::string m_matFileSuffix;
	std::string m_storagePath;		// 用于存储的最终路径。debug用。
public:
	CStorage();
	~CStorage();
	bool Store(cv::Mat *pictures, int num);		// 存储图片。
	
	bool SetMatFileName(std::string matFilePath,	// 设定存储路径并创建
		std::string matFileName,
		std::string matFileSuffix);
};

// 可视化模块，用于debug。自动创建销毁窗口。
class CVisualization
{
private:
	std::string m_winName;		// 窗口名称
public:
	CVisualization(std::string winName);
	~CVisualization();
	int Show(cv::Mat pic, int time, bool norm = false, double zoom = 1.0);
};

// 格雷码解码器。解码已经编写好的格雷码。
// 输入为一组灰度图，输出为一张灰度图，每一点存储的是projector中的坐标。
// 使用前需要分别调用4个Set函数传参。
class CDecode_Gray
{
private:
	int m_numDigit;			// 位数
	int m_grayCodeSize;		// 总共的格雷码数目
	short * m_gray2bin;		// 格雷码到二进制码的转换
	std::string m_codeFilePath;	// 存储格雷码的文件路径
	std::string m_codeFileName;	// 存储格雷码的文件名
	int resRow;				// 图像的行分辨率
	int resLine;			// 图像的列分辨率
	bool m_vertical;		// 设定格雷码方向

	cv::Mat * m_grePicture;	// 输入的灰度图
	cv::Mat * m_binPicture;	// 加工后的二值图
	cv::Mat m_result;		// 结果

	CVisualization * m_visual;	// 用于显示中间结果

	bool AllocateSpace();		// 为输入的矩阵、short类型内容申请空间
	bool ReleaseSpace();		// 删除所有空间
	bool Grey2Bin();			// 将灰度图加工为二值图，以便进一步处理
	bool CountResult();			// 根据二值图统计结果
	bool Visualize();			// 显示中间结果

public:
	CDecode_Gray();
	~CDecode_Gray();

	bool Decode();
	cv::Mat GetResult();

	bool SetMat(int num, cv::Mat pic);						// 输入相应灰度图
	bool SetNumDigit(int numDigit, bool ver);				// 设置格雷码位数
	bool SetMatFileName(std::string codeFilePath,
		std::string codeFileName);			// 设置存储格雷码的文件名
};

// 相移解码器。解码已经编写好的相移码。
// 输入为一组灰度图，输出为一张灰度图，每一点存储的是projector中的偏移坐标。
class CDecode_Phase
{
private:
	int m_numMat;		// 图片数目。默认为2。
	int m_pixPeroid;	// 像素周期。
	int m_resRow;				// 图像的行分辨率
	int m_resLine;			// 图像的列分辨率

	cv::Mat * m_grePicture;	// 输入的灰度图
	cv::Mat m_result;		// 结果

	CVisualization * m_visual;	// 用于显示中间结果

	bool AllocateSpace();		// 为输入的矩阵申请空间
	bool DeleteSpace();			// 删除所有空间
	bool CountResult();			// 根据归一化灰度图统计结果
	bool Visualize();			// 显示中间结果

public:
	CDecode_Phase();
	~CDecode_Phase();

	bool Decode();
	cv::Mat GetResult();

	bool SetMat(int num, cv::Mat pic);						// 输入相应灰度图
	bool SetNumMat(int numDigit, int pixperiod);			// 设置参数
};

// 标定类。用于标定摄像头、投影仪。
// 其中会引用摄像头和投影仪的控制，并创建两个解码器进行解码。
class CCalibration
{
private:
	// 传感器控制类
	CSensor * m_sensor;
	// 解码器
	CDecode_Gray * m_decodeGray;	// 格雷码解码
	CDecode_Phase * m_decodePS;		// PS解码
	// 棋盘格的X、Y，以及图片数目
	int m_chessLine;
	int m_chessRow;
	int m_chessNum;
	// 采集到的棋盘格图像和识别结果
	cv::Mat m_chessMat;
	cv::Mat * m_grayV;
	cv::Mat * m_grayH;
	cv::Mat * m_phaseV;
	cv::Mat * m_phaseH;
	cv::Mat m_chessMatDraw;
	cv::Mat m_proMatDraw;
	//棋盘格顺序编号
	std::vector<std::vector<cv::Point2f>> m_camPoint;
	std::vector<cv::Point2f> m_camPointTmp;
	std::vector<std::vector<cv::Point2f>> m_proPoint;
	std::vector<cv::Point2f> m_proPointTmp;
	std::vector<std::vector<cv::Point3f>> m_objPoint;
	std::vector<cv::Point3f> m_objPointTmp;
	// 标定的矩阵
	cv::Mat m_camMatrix;
	cv::Mat m_camDisCoeffs;
	cv::Mat m_proMatrix;
	cv::Mat m_proDisCoeffs;
	cv::Mat m_R;
	cv::Mat m_T;
	cv::Mat m_E;
	cv::Mat m_F;

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