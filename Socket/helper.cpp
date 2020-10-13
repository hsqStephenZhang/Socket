#pragma once

#include "helper.h"
#include "Config.h"
#include <Windows.h>

bool initConf(Config &config) {
	int port;
	std::string ipAddress;
	std::string username;
	std::string password;

	port = config.Read("port", 0);
	ipAddress = config.Read("ipAddress", ipAddress);
	username = config.Read("username", username);
	password = config.Read("password", password);

	setColor(YELLOW);
	std::cout << "port:" << port << std::endl;
	std::cout << "ipAddress:" << ipAddress << std::endl;
	std::cout << "username:" << username << std::endl;
	std::cout << "password:" << password << std::endl;

	return true;
}

void getList(string &str, char pattern, vector<string> &res) {
	res.clear();
	if (str == "") return ;
	string strs = str + pattern;
	size_t pos = strs.find(pattern);

	while (pos != strs.npos)
	{
		string temp = strs.substr(0, pos);
		temp.erase(0, temp.find_first_not_of(" "));
		temp.erase(temp.find_last_not_of(" ") + 1);
		res.push_back(temp);
		//去掉已分割的字符串,在剩下的字符串中进行分割
		strs = strs.substr(pos + 1, strs.size());
		pos = strs.find(pattern);
	}
}

void GetAllFiles(string path, vector<string>& files)
{
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	while (path[path.length() - 1] == '\\') {
		path.pop_back();
	}
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					GetAllFiles(p.assign(path).append("\\").append(fileinfo.name), files);
				}
			}
			else
			{
				files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}

		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}
}

bool matchPattern(vector<string>&patterns, string filepath) {
	string suffix = filepath.substr(filepath.find_last_of('.') + 1);
	for (auto &pattern : patterns) {
		if (pattern == suffix) return true;
	}
	return false;
}

void GetAllFiles(string path, vector<string>&patterns, vector<string>& files)
{
	long   hFile = 0;
	//文件信息  
	struct _finddata_t fileinfo;
	while (path[path.length() - 1] == '\\') {
		path.pop_back();
	}
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib &  _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					if (matchPattern(patterns, fileinfo.name)) {
						files.push_back(p.assign(path).append("\\").append(fileinfo.name));
					}					
					GetAllFiles(p.assign(path).append("\\").append(fileinfo.name),patterns, files);
				}
			}
			else
			{
				if (matchPattern(patterns, fileinfo.name)) 
					files.push_back(p.assign(path).append("\\").append(fileinfo.name));
			}

		} while (_findnext(hFile, &fileinfo) == 0);

		_findclose(hFile);
	}
}

string& replace_all(string& src, const string& old_value, const string& new_value) {
	// 每次重新定位起始位置，防止上轮替换后的字符串形成新的old_value
	for (string::size_type pos(0); pos != string::npos; pos += new_value.length()) {
		if ((pos = src.find(old_value, pos)) != string::npos) {
			src.replace(pos, old_value.length(), new_value);
		}
		else break;
	}
	return src;
}

void setColor(Colors color) {
	switch (color)
	{
	case Colors::RED: {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED);
		break;
	}
	case Colors::YELLOW: {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED |
			FOREGROUND_GREEN);
		break;
	}
	case Colors::GREEN: {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN);
		break;
	}
	case Colors::WHITE: {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED |
			FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	}
	default: {
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED |
			FOREGROUND_GREEN | FOREGROUND_BLUE);
		break;
	}
	}
}