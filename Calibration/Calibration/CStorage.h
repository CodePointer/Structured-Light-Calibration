#ifndef _CSTORAGE_H_
#define _CSTORAGE_H_

#include <iostream>
#include <string>
#include <strstream>
#include <opencv2\opencv.hpp>
#include "GlobalFunction.h"
using namespace std;
using namespace cv;

// 数据存储模块。存储中间数据做存档
class CStorage
{
private:
	string m_matFilePath;
	string m_matFileName;
	string m_matFileSuffix;
	string m_storagePath;		// 用于存储的最终路径。debug用。
public:
	CStorage();
	~CStorage();
	bool Store(Mat *pictures, int num);		// 存储图片。

	bool SetMatFileName(std::string matFilePath,	// 设定存储路径并创建
		std::string matFileName,
		std::string matFileSuffix);
};

#endif