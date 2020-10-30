#pragma once
#include "http/http_conn.h"
#include "helper.h"
#include <Windows.h>
#include <conio.h>

using namespace std;

#define MAX_SIZE 4096

Http::Http(Config config) {
	// 初始化 socketaddr_in
	this->sin.sin_port = htons(config.Read("port", 8888));
	this->sin.sin_family = AF_INET;
	this->sin.sin_addr.S_un.S_addr = INADDR_ANY;

	// 初始化用户配置信息
	this->username = config.Read("username", string("stephenzhang"));
	this->password = config.Read("password", string("123456"));
	this->rootDir = config.Read("rootDir", string("E:\\DESKTOP\\VSproject\\Socket\\Socket\assets"));
	this->errorPage = config.Read("errorPage", string("/error404.html"));
	this->authorized = false;

	// 初始化controllers 映射
	for (auto iter = config.m_Contents.begin(); iter != config.m_Contents.end(); iter++) {
		if (iter->first[0] == '/') this -> controllers[iter->first] = iter->second;
	}
	// 初始化filters
	string f;  // filters
	f= config.Read("filters", f);
	getList(f, ',', filters);
}

bool Http::init() {
	setColor(YELLOW);
	cout << "[http server initlizing...]" << endl;

	WORD sockVersion = MAKEWORD(2, 2);
	WSADATA wsaData;
	if (WSAStartup(sockVersion, &wsaData) != 0)  return false;
	slisten = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (slisten == INVALID_SOCKET) {
		setColor(RED);
		cout << "invalid socket" << endl;
		return false;
	}
	else {
		if (bind(slisten, (LPSOCKADDR)&sin, sizeof(sin)) == SOCKET_ERROR)
		{
			setColor(RED);
			cout << "bind error !" << endl;
			return false;
		}
		if (listen(slisten, 5) == SOCKET_ERROR) {
			setColor(RED);
			cout << "listen error!" << endl;
			return false;
		}

		setColor(YELLOW);
		cout << "[http server initialized]\n" << endl;
		return true;
	}
}

void Http::loop() {
	string url;
	char revData[MAX_SIZE]={0};
	int nAddrlen = sizeof(remoteAddr);
	
	sockaddr_in remoteAddr;
	HttpMethods method;

	int keyboard;

	while (true) {
		setColor(WHITE);
		cout << endl<< "waiting for connection...\n";

		sClient = accept(this->slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
		if (sClient == INVALID_SOCKET)
		{
			setColor(RED);
			printf("accept error !");
			continue;
		}
		else {
			// 输出建立的连接的信息
			setColor(WHITE);
			printf("connection build, address: %s,  port: %d", inet_ntoa(remoteAddr.sin_addr), remoteAddr.sin_port);
			int ret = recv(sClient, revData, MAX_SIZE, 0);
			coutHead(revData);

			// 解析输入的网址
			map<string, string> args;
			// 找到对应的处理的方法，接收对应的参数
			parseRequest(revData, method, url,args);
			// 对应不同的method，有不同的处理方法
			handleRequest(revData,method,url,args);
		}
		closesocket(sClient);
	}
	closesocket(slisten);
	WSACleanup();
	return;
}

void Http::requestMapping(string url, string resource) {
	if (controllers.find(url) != controllers.end()) return;
	else {
		controllers[url]=resource;
	}
}

void Http::parseRequest(char *head, HttpMethods &method, string &url, std::map<string, string> &args) {
	string type;
	int i = 0;
	char currentChar;
	while ((currentChar = *(head + i)) != ' ') {
		type.push_back(currentChar);
		i++;
	}
	i++;
	if (type.compare("GET") == 0) method = HttpMethods::_GET;
	else if (type.compare("POST") == 0) method = HttpMethods::_POST;
	else if (type.compare("PUT") == 0) method = HttpMethods::_PUT;
	else if (type.compare("HEAD") == 0) method = HttpMethods::_HEAD;
	else if (type.compare("DELETE") == 0) method = HttpMethods::_DELETE;
	
	
	// 获取请求的方式和请求的url
	url.clear();
	while ((currentChar = *(head + i)) != ' ') {
		url.push_back(currentChar);
		i++;
	}
	if (strcmp(url.c_str(), "  ") == 0) {
		url.clear();
	}
	setColor(YELLOW);
	cout <<"[request type:"<< type<<" request url:"<<url <<" ]"<< endl;

	// 处理post,delete请求对应的参数
	if (method == HttpMethods::_POST || method == HttpMethods::_DELETE)
	{
		char *pos = strstr(head, "Content-Length:");
		pos += 16;
		int length = 0;
		i = 0;
		char c;
		while ((c = pos[i]) != '\r' && c != '\n') {
			length = length * 10;
			length += c - '0';
			i++;
		}
		// post请求附带的正文的内容从content开始
		char *content = strstr(head, "\r\n\r\n");
		content += 4;
		parseArgs(content, args);
		authorize(args);
	}
}

void Http::handleRequest(char *revData,HttpMethods &method, string &url, std::map<string, string> &args) {
	if (method == HttpMethods::_GET) {
		handleGet(revData, url);
	}
	else if (method == HttpMethods::_DELETE) {
		handleDelete(revData, url);
	}
	else if (method == HttpMethods::_POST) {
		handlePost(revData, url, args);
	}
}

void Http::handleGet(char *revData, string &url) {
	map<string, string>args;
	int pos;
	if ((pos= url.find('?')) != url.npos) {
		parseArgs(url.c_str()+pos+1,args);
		url = string(url.begin(), url.begin() + pos);
	}

	auto res = this->controllers.find(url);
	// 找到了对应的静态资源
	if (res != this->controllers.end()) {
		setColor(YELLOW);
		cout << "[find " + url + " success]" << endl;
		sendResponse(revData,url);
	}
	else {
		// 过滤请求
		for (auto iter = this->filters.begin(); iter != this->filters.end(); iter++) {
			if (url == *iter) {
				setColor(GREEN);
				cout << "[filter rosource:  " << url << " ]" << endl;
				return;
			}
		}

		setColor(RED);
		cout << "[fail to locate resource]" << url << endl;
		cout << "[now send 404 not found error]" << endl;
		redirect(this->errorPage);
	}
}

void Http::sendResponse(char *revData, string & url) {
	if (strstr(revData, "text/html"))
	{
		cout << "now send a html: " << this->controllers[url] << endl;
		sendHtml(rootDir + replace_all(this->controllers[url], "/", "\\"));
	}
	else if (strstr(revData, "Sec-Fetch-Dest: image")) {
		cout << "now send a image: " << this->controllers[url] << endl;
		sendMedia(rootDir + replace_all(this->controllers[url], "/", "\\"));
	}
}

void Http::handleDelete(char *revData, string &url) {
	cout << revData << endl;
	redirect("/index.html");
}

void Http::handlePost(char *revData, string &url,map<string, string> &args) {
	bool res = authorize(args);
	if (res) {
		setColor(RED);
		cout << endl << "*******************authorized***************" << endl << endl;
	}
	redirect("/index.html");
}

bool Http::authorize(std::map<string, string> &args) {
	bool res=false;
	auto iter1 = args.find("username");
	auto iter2= args.find("password");
	if (iter1!=args.end() && iter2 != args.end()) {
		res = iter1->second == this->username && iter2->second == this->password;
	}
	if (res) this->authorized = true;
	return res;
}

// 重定向，这里给出的url，不需要在controllers里面查找
void Http::redirect(const char *url) {
	string u = url;
	redirect(u);
}

void Http::redirect(string &url) {
	sendHtml(rootDir + replace_all(url, "/", "\\"));
}

void Http::sendHtml(string filename) {
	char content[1024 * 64] = { 0 };

	FILE*fp;
	fp = fopen(filename.c_str(), "rb");
	fseek(fp, 0, SEEK_END);
	long lSize = ftell(fp);
	rewind(fp);
	char *buf = new char[lSize];
	fread(buf, 1, lSize, fp);

	int size = sprintf(content, head.c_str(), lSize);
	//cout << content << endl;
	strcat(content + size, buf);
	content[size + lSize] = '\0';
	//cout << content << endl;

	send(sClient, content, size + lSize + 1, 0);
	delete buf;
}

void Http::sendMedia(string filename) {
	char content[1024 * 1024] = { 0 };

	FILE*fp;
	fp = fopen(filename.c_str(), "rb");
	fseek(fp, 0, SEEK_END);
	long lSize = ftell(fp);
	rewind(fp);
	char *buf = new char[lSize];
	fread(buf, 1, lSize, fp);

	int size = sprintf(content, mediaHead.c_str(), lSize);
	//cout << content << endl;
	memcpy(content + size, buf, lSize);
	content[lSize + size] = '\0';
	//cout << content << endl;
	send(sClient, content, size + lSize + 1, 0);
	delete buf;
}
