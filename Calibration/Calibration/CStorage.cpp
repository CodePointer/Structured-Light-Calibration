#include "head.h"
#include <strstream>

CStorage::CStorage()
{
	this->m_matFilePath = "";
	this->m_matFileName = "";
	this->m_matFileSuffix = "";
	this->m_storagePath = "";
}

CStorage::~CStorage()
{

}

bool CStorage::Store(cv::Mat * pictures, int num)
{
	using namespace cv;

	// 判断参数是否合法
	if (num <= 0)
		return false;

	bool status = true;

	// 存储
	if (num == 1)
	{
		status = imwrite(this->m_storagePath, *pictures);
		if (!status)
		{
			// 创建目录
			string temp = this->m_matFilePath;
			for (int i = 0; i < temp.length(); i++)
			{
				if (temp[i] == '/')
					temp[i] = '\\';
			}
			system((string("mkdir ") + temp).c_str());
			status = imwrite(this->m_storagePath, *pictures);
		}
	}
	else
	{
		for (int i = 0; i < num; i++)
		{
			std::string tempNum;
			std::strstream ss;
			ss << i;
			ss >> tempNum;
			status = imwrite(this->m_storagePath, pictures[i]);
			if (!status)
			{
				// 创建目录
				string temp = this->m_matFilePath;
				for (int i = 0; i < temp.length(); i++)
				{
					if (temp[i] == '/')
						temp[i] = '\\';
				}
				system((string("mkdir ") + temp).c_str());
				status = imwrite(this->m_storagePath, pictures[i]);
			}
		}
	}

	if (!status)
	{
		ErrorHandling("CStorage.Store->imwrite Error.");
	}

	return true;
}

// 设定存储目录
bool CStorage::SetMatFileName(std::string matFilePath,
	std::string matFileName,
	std::string matFileSuffix)
{
	using namespace std;

	// 变更参数
	this->m_matFilePath = matFilePath;
	this->m_matFileName = matFileName;
	this->m_matFileSuffix = matFileSuffix;
	this->m_storagePath = matFilePath + matFileName + matFileSuffix;

	//// string转换
	//string temp = matFilePath;
	//for (int i = 0; i < temp.length(); i++)
	//{
	//	if (temp[i] == '/')
	//		temp[i] = '\\';
	//}

	//// 创建目录
	//system((string("mkdir ") + temp).c_str());
	
	return true;
}