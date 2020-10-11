#pragma once
#include "http/http_conn.h"
//#include "logger.h"

using namespace std;

#define MAX_SIZE 4096

Http::Http(Config config) {
	// 初始化 socketaddr_in
	this->sin.sin_port = htons(config.getPort());
	this->sin.sin_family = AF_INET;
	this->sin.sin_addr.S_un.S_addr = INADDR_ANY;
	// 初始化buffer的大小
	this->BUFFER_SIZE = config.getbufferSize();
	getMappings(config.getControllerPath());
}

bool Http::init() {
	cout << "[http server initlizing...]" << endl;

	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return false;
	}
	slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET) {
		cout << "invalid socket" << endl;
		return false;
	}
	else {
		if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
		{
			cout << "bind error !" << endl;
			return false;
		}
		if (listen(slisten, 5) == SOCKET_ERROR) {
			cout << "listen error!" << endl;
			return false;
		}

		nAddrlen = sizeof(remoteAddr);
		cout << "[http server initialized]\n" << endl;
		return true;
	}
}

void Http::loop() {
	SOCKET sClient;
	sockaddr_in remoteAddr;
	int nAddrlen = sizeof(remoteAddr);
	char revData[MAX_SIZE];
	while (true) {
		printf("waiting for connection...\n");
		sClient = accept(this->slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
		if (sClient == INVALID_SOCKET)
		{
			printf("accept error !");
			continue;
		}
		printf("connection build：%s \r\n", inet_ntoa(remoteAddr.sin_addr));
		int ret = recv(sClient, revData, MAX_SIZE, 0);
		//cout << revData << endl;
		HttpMethods method;
		string url;
		parseRequest(revData,method,url);
		if (this->controllers.find(url) != this->controllers.end()) {
			cout << "[find success]" << endl;
		}
		else {
			cout << "[fail to locate resource]" << endl;
			cout << "[now send 404 not found error]" << endl;
		}
		//logger(url);

		// 请求头之后必须要一个空行，标志结束
		//char sendData[] = "HTTP / 1.1 200 OK\r\n"
		//	"Server: hsoap/2.8\r\n"
		//	"Content-Type: text/html\r\n"
		//	"Content-Length: 10\r\n"
		//	"Connection: close\r\n\r\n"
		//	"<h1>hello world</h1><h2>this is head2</h2><h2>this is head2</h2><h2>this is head2</h2>";

		string sendData = "";
		sendData += "HTTP / 1.1 200 OK\r\n"
			"Server: hsoap/2.8\r\n"
			"Content-Type: text/html\r\n"
			"Content-Length: 20\r\n"
			"Connection: close\r\n\r\n";
		sendData += "<h1>hello world</h1><h2>this is head2</h2><h2>this is head2</h2><h2>this is head2</h2>";

		//cout << revData;
		//HttpMethods method;
		//string  str;
		//parseRequest(revData, method, str);

		if (strstr(revData, "GET /test")) {
			cout << "now send a html" << endl;
			send(sClient, sendData.c_str(), sendData.size(), 0);
		}

		closesocket(sClient);
	}
	closesocket(slisten);
	WSACleanup();
	return;
}

void Http::getMappings(string filename) {
	char buffer[256] = {0};
	ifstream fp(filename);
	if (!fp.is_open()) return ;
	while (!fp.eof()) {
		fp.getline(buffer, 255);
		string a = "";
		string b = "";
		int i = 0;
		char c;
		while (buffer[i] == ' ') i++;
		while ((c = buffer[i++]) != ':') {
			a.push_back(c);
		}
		while (buffer[i] == ' ') i++;
		while ((c = buffer[i++]) != ' ' && (c != '\n') && (c != '\0')) {
			b.push_back(c);
		}
		if (strcmp(a.c_str(), "rootdir")==0) {
			
		}

		controllers[a] = b;
	}
}

void Http::requestMapping(string url, string resource) {
	if (controllers.find(url) != controllers.end()) return;
	else {
		controllers.insert(make_pair(url, resource));
	}
}

void Http::parseRequest(char *head, HttpMethods &method, string &url) {
	string type;
	int i = 0;
	char currentChar;
	while ((currentChar = *(head + i)) != ' ') {
		type.push_back(currentChar);
		i++;
	}
	if (type.compare("GET") == 0) method = HttpMethods::_GET;
	if (type.compare("POST") == 0) method = HttpMethods::_POST;
	if (type.compare("PUT") == 0) method = HttpMethods::_PUT;
	if (type.compare("HEAD") == 0) method = HttpMethods::_HEAD;
	if (type.compare("DELETE") == 0) method = HttpMethods::_DELETE;
	cout << type << endl;

	i++;
	url.clear();
	while ((currentChar = *(head + i)) != ' ') {
		url.push_back(currentChar);
		i++;
	}
	cout << url << endl;
}