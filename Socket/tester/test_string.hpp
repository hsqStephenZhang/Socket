#pragma once


#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <istream>
#include <io.h>
#include "../helper.h"

using namespace std;

void test1() {
	string a = "123";
	string b = "456";
	auto c = a.data();
	auto d = a.c_str();
	cout << c << endl;
	cout << a+b << endl;
}

void test2() {
	string a = "123";
	char b[] = "123";
	cout << strlen(a.c_str()) << endl;
	cout << strlen(b) << endl;
}

void test3() {
	string a = "123";
	cout << a[a.length()-1] << endl;
}

void test4() {
	vector<string> res;
	vector<string>patterns;
	patterns.push_back(string("html"));
	//patterns.push_back(string("txt"));
	GetAllFiles("E:\\DESKTOP\\VSproject\\Socket\\x64\\Debug\\", patterns, res);
	for (auto &str : res) {
		cout << str << endl;
	}
}

void test5() {
	std::map<string, string> res;
	char args[] = "args1=123&arg2=456";
	parseArgs(args, res);
	for (auto iter : res) {
		cout << iter.first << ":" << iter.second << endl;
	}
}

void test6() {
	string url = "/index.html?args1=123&arg2=456";
	int pos;
	map<string, string>args;
	if ((pos = url.find('?')) != url.npos) {
		parseArgs(url.c_str() + pos + 1, args);
	}
	for (auto iter : args) {
		cout << iter.first << ":" << iter.second << endl;
	}
	string sub(url.begin(),url.begin()+pos);
	cout << sub << endl;
}

