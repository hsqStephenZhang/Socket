#pragma once

#include <iostream>
#include <istream>
#include <fstream>
#include <io.h>
#include <vector>
#include <string>
#include "Config.h"

using std::string;
using std::vector;
using std::cout;
using std::endl;

enum  Colors
{
	RED,
	YELLOW,
	GREEN,
	WHITE,
};

bool initConf(Config &config);

void getList(string &str, char split,vector<string> &res);

string& replace_all(string& src, const string& old_value, const string& new_value);

void GetAllFiles(string path, vector<string>& files);

bool matchPattern(vector<string>&patterns, string filepath);

void GetAllFiles(string path, vector<string>&patterns, vector<string>& files);

void setColor(Colors color);