#pragma once
#include <iostream>
#include <fstream>
#include <ostream>
#include <istream>
#include <string>
#include <vector>
#include <map>
#include <set>
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
"Connection: keep-alive\r\n\r\n";

const string mediaHead = "HTTP / 1.1 200 OK\r\n"
"accept-ranges: bytes\r\n"
"Server: hsoap/2.8\r\n"
"Content-Type: image/jpg \r\n"
"Content-Length: %d\r\n\r\n";

class Http {
private:
	SOCKET slisten;
	sockaddr_in sin;
	sockaddr_in remoteAddr;
	SOCKET sClient;

	string username;
	string password;
	string rootDir;
	string errorPage;
	bool authorized;

	map<string, string> controllers;
	vector<string> filters;

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
	// 解析请求

	void redirect(string &url);
	void redirect(const char *url);
	void sendHtml(string filename);
	void sendMedia(string filename);
	void sendResponse(char *revData,string & url);
	void parseRequest(char *head, HttpMethods &method, string &url, std::map<string, string> &args);
	void handleRequest(char *revData,HttpMethods &method,string &url, std::map<string, string> &args);
	void handleGet(char *revData,string &url);
	void handleDelete(char *revData, string &url);
	void handlePost(char *revData, string &url, std::map<string, string> &args);
	bool authorize(std::map<string, string> &args);

	~Http(){}
};



