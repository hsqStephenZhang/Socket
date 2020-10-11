#pragma once

#include <iostream>
#include <istream>
#include <fstream>
#include <io.h>
#include <vector>
#include <string>

using std::string;
using std::vector;
using std::cout;
using std::endl;

void GetAllFiles(string path, vector<string>& files);

bool matchPattern(vector<string>&patterns, string filepath);

void GetAllFiles(string path, vector<string>&patterns, vector<string>& files);