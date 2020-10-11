#pragma once
#include <iostream>
#include <fstream>
#include <ostream>
#include <istream>
#include <string>
#include <vector>
#include <map>
#include <winsock2.h>  
#pragma comment(lib,"ws2_32.lib")  

#include "../config.h"
//#include "../logger.h"



using namespace std;

const string ok_200_title = "OK";
const string error_400_title = "Bad Request";
const string error_400_form = "Your request has bad syntax or is inherently impossible to staisfy.";
const string error_403_title = "Forbidden";
const string error_403_form = "You do not have permission to get file form this server.";
const string error_404_title = "Not Found";
const string error_404_form = "The requested file was not found on this server.";
const string error_500_title = "Internal Error";
const string error_500_form = "There was an unusual problem serving the request file.";

const string head = "HTTP / 1.1 200 OK\r\n"
"Server: hsoap/2.8\r\n"
"Content-Type: text/html\r\n"
"Content-Length: %d\r\n"
"Connection: close\r\n\r\n";

class Http {
private:
	SOCKET slisten;
	sockaddr_in sin;
	SOCKET sClient;
	sockaddr_in remoteAddr;
	int BUFFER_SIZE;
	int nAddrlen;
	map<string, string> controllers;
	enum HttpMethods
	{
		_GET,
		_POST,
		_PUT,
		_HEAD,
		_DELETE,
		_TRACE
	};

public:
	Http() {}
	Http(Config config);
	bool init();
	void loop();

	// 添加映射
	void requestMapping(string url, string resource);
	void getMappings(string filename);

	// 解析请求
	void parseRequest(char *head, HttpMethods &method, string &url);

	void sendHtml();
	void sendMedia();

	~Http(){}
};