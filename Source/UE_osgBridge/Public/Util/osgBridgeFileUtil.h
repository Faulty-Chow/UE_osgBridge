#pragma once
#include <string>
#include <vector>
#include <initializer_list>

class UE_OSGBRIDGE_API osgBridgeFileUtil
{
public:
	static std::string PathCombine(std::initializer_list<std::string>&& pathList);
	static bool IsFileExist(const std::string& filePath);
	static std::vector<std::string> GetFolders(const std::string& dirPath);
	static std::vector<std::string> GetFiles(const std::string& dirPath);
};