#include "Util/osgBridgeFileUtil.h"
#include <cstdarg>
#include <algorithm>
#include <sys/stat.h>
#include <io.h>

std::string osgBridgeFileUtil::PathCombine(std::initializer_list<std::string>&& pathList)
{
	std::string result;
	for (auto path : pathList)
	{
		std::replace(path.begin(), path.end(), '/', '\\');
		if (result.empty())
			result = result.assign(path);
		else if (result.back() != '\\')
			result = result.append("\\").append(path);
		else
			result = result.append(path);
	}
	return result;
}

bool osgBridgeFileUtil::IsFileExist(const std::string& filePath)
{
	struct stat buffer;
	return stat(filePath.c_str(), &buffer) == 0;
}

std::vector<std::string> osgBridgeFileUtil::GetFolders(const std::string& dirPath)
{
	std::vector<std::string> result;
	long long handle = 0;
	struct _finddata_t fileinfo;
	std::string temp_str;
	if ((handle = _findfirst(temp_str.assign(dirPath).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					result.emplace_back(fileinfo.name);
				}
			}
		} while (_findnext(handle, &fileinfo) == 0);
	}
	_findclose(handle);
	return result;
}

std::vector<std::string> osgBridgeFileUtil::GetFiles(const std::string& dirPath)
{
	std::vector<std::string> result;
	long long handle = 0;
	struct _finddata_t fileinfo;
	std::string temp_str;
	if ((handle = _findfirst(temp_str.assign(dirPath).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if (!(fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					result.emplace_back(fileinfo.name);
				}
			}
		} while (_findnext(handle, &fileinfo) == 0);
	}
	_findclose(handle);
	return result;
}