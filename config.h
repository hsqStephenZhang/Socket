#pragma once

#include <string>

using namespace std;

class Config {
private:
	int MaxConnection;		//最大连接数 ********暂时用不到
	int bufferSize;		//缓冲区大小
	int port;				//服务器端口
	string addr;  //服务器地址
	string rootdir; // 根地址
	string controllerPath; // 映射表路径

public:
	Config(int MaxConnection = 10, int bufferSize = 4096, int port = 8888, string addr = "127.0.0.1",string rootdir="..\\assets",string controllerPath="E:\\DESKTOP\\VSproject\\Socket\\x64\\Debug\\web.conf") {
		this->MaxConnection = MaxConnection;
		this->bufferSize = bufferSize;
		this->port = port;
		this->addr = addr;
		this->rootdir = rootdir;
		this->controllerPath = controllerPath;
	}

	void setConnectionNum(int num) {
		this->MaxConnection = num;
	}

	void setbufferSize(int bufferSize) {
		this->bufferSize = bufferSize;
	}

	void setAddr(string addr) {
		this->addr = addr;
	}

	void setPort(int port) {
		this->port = port;
	}

	void setControllerPath(string controllerPath) {
			this->controllerPath = controllerPath;
	}

	int getConnectionNum() {
		return this->MaxConnection;
	}

	int getbufferSize() {
		return this->bufferSize;
	}

	int getPort() {
		return this->port;
	}

	string getAddr() {
		return this->addr;
	}

	string getrootdir() {
		return this->rootdir;
	}

	string getControllerPath() {
		return this->controllerPath;
	}
};