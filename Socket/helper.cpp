#pragma once

#include "helper.h"

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
