#pragma once

#include <iostream>
#include <fstream>
#include <ostream>
#include <istream>
#include <string>

#define LOG_TO_FILE 0
#define LOG_TO_CONSOLE 1

using std::string;
using std::cout;
using std::endl;

const string sep(20,'*');

void logger(string log)
{
	cout << sep << endl;
	cout << log << endl;
	cout << sep << endl;
}

void logger(string log, string filename)
{
	std::ofstream fp;
	fp.open(filename.c_str());
	fp << sep << endl;
	fp << log << endl;
	fp << sep << endl;
}