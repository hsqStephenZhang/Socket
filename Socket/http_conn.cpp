#pragma once
#include "http/http_conn.h"
#include "helper.h"
#include <Windows.h>
//#include "logger.h"

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
	char revData[MAX_SIZE];
	int nAddrlen = sizeof(remoteAddr);
	
	sockaddr_in remoteAddr;
	HttpMethods method;

	while (true) {
		setColor(RED);
		printf("waiting for connection...\n");
		sClient = accept(this->slisten, (SOCKADDR *)&remoteAddr, &nAddrlen);
		if (sClient == INVALID_SOCKET)
		{
			setColor(RED);
			printf("accept error !");
			continue;
		}
		else {
			setColor(WHITE);
			printf("connection build：%s \r\n", inet_ntoa(remoteAddr.sin_addr));

			int ret = recv(sClient, revData, MAX_SIZE, 0);

			// 解析输入的网址
			parseRequest(revData, method, url);
			// 对应不同的method，有不同的处理方法
			if (method == HttpMethods::_GET) {
				handleGet(revData, url);
			}
			else if (method == HttpMethods::_DELETE) {
				handleDelete(revData, url);
			}
			else if (method == HttpMethods::_POST) {
				handlePost (revData, url);
			}
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

void Http::parseRequest(char *head, HttpMethods &method, string &url) {
	string type;
	int i = 0;
	char currentChar;
	while ((currentChar = *(head + i)) != ' ') {
		type.push_back(currentChar);
		i++;
	}
	if (type.compare("GET") == 0) method = HttpMethods::_GET;
	else if (type.compare("POST") == 0) method = HttpMethods::_POST;
	else if (type.compare("PUT") == 0) method = HttpMethods::_PUT;
	else if (type.compare("HEAD") == 0) method = HttpMethods::_HEAD;
	else if (type.compare("DELETE") == 0) method = HttpMethods::_DELETE;
	cout << type << endl;

	i++;
	url.clear();
	while ((currentChar = *(head + i)) != ' ') {
		url.push_back(currentChar);
		i++;
	}
	if (strcmp(url.c_str(), "  ") == 0) {
		url.clear();
	}
	cout << url << endl;
}

void Http::handleGet(char *revData, string &url) {
	auto res = this->controllers.find(url);
	if (res != this->controllers.end()) {
		setColor(YELLOW);
		cout << "[find " + url + " success]" << endl;
		if (strstr(revData, "GET")) {
			if (strstr(revData, "text/html"))
			{
				cout << "now send a html" << endl;
				cout << this->controllers[url] << endl;
				sendHtml(rootDir + replace_all(this->controllers[url], "/", "\\"));
			}
			else if (strstr(revData, "Sec-Fetch-Dest: image")) {
				cout << "now send a jpg" << endl;
				sendMedia(rootDir + replace_all(this->controllers[url], "/", "\\"));
			}
		}
	}
	else {

		// 过滤请求
		for (auto iter = this->filters.begin(); iter != this->filters.end(); iter++) {
			if (url == *iter) return;
		}

		setColor(RED);
		cout << "[fail to locate resource]" << endl;
		cout << "[now send 404 not found error]" << endl;
		redirect(this->errorPage);
	}
}

void Http::handleDelete(char *revData, string &url) {
	cout << revData << endl;
	redirect("/index.html");
}

void Http::handlePost(char *revData, string &url) {
	bool res = authorize(revData);
	if (res) cout << "*********************" << endl;
	redirect("/index.html");
}

bool Http::authorize(char *revData) {
	setColor(RED);
	char line[256];
	char *username = strstr(revData, "username");
	if (username != nullptr) {
		char *password = strstr(username, "password");
		if (password != nullptr) {
			string usernm;
			string passwd;
			username += 9;
			int i = 0;
			char c;
			while ((c = *(username + i)) != '&') {
				usernm.push_back(c);
				i++;
			}

			// username 匹配成功
			if (usernm == this->username) {
				i = 0;
				password += 9;
				// 开始匹配password
				while ((c = *(password + i))) {
					passwd.push_back(c);
					if ((i < this->password.size() ) && (c == this->password[i])) {
						i++;
					}
					else {
						break;
					}
				}
				if (i == this->password.size()) {
					setColor(YELLOW);
					cout << "username:" + usernm << endl;
					cout << "password:" + passwd << endl;
					this->authorized = true;
					return true;
				}
				else {
					setColor(RED);
					cout << "authorize failed" << endl;
				}
			}
		}
	}
}

void Http::redirect(const char *url) {
	string u = url;
	redirect(u);
}

void Http::redirect(string &url) {
	sendHtml(rootDir + replace_all(this->controllers[url], "/", "\\"));
}

void Http::sendHtml(string filename) {
	char content[1024 * 64] = { 0 };

	FILE*fp;
	cout << filename << endl;
	fp = fopen(filename.c_str(), "rb");
	fseek(fp, 0, SEEK_END);
	long lSize = ftell(fp);
	rewind(fp);
	char *buf = new char[lSize];
	fread(buf, 1, lSize, fp);

	int size = sprintf(content, head.c_str(), lSize);
	cout << content << endl;
	strcat(content + size, buf);
	content[size + lSize] = '\0';
	cout << content << endl;

	send(sClient, content, size + lSize + 1, 0);
	delete buf;
}

void Http::sendMedia(string filename) {
	char content[1024 * 1024] = { 0 };

	FILE*fp;
	//fp = fopen(filename.c_str(), "rb");
	cout << filename << endl;
	fp = fopen(filename.c_str(), "rb");
	fseek(fp, 0, SEEK_END);
	long lSize = ftell(fp);
	rewind(fp);
	char *buf = new char[lSize];
	fread(buf, 1, lSize, fp);

	int size = sprintf(content, mediaHead.c_str(), lSize);
	cout << content << endl;
	memcpy(content + size, buf, lSize);
	content[lSize + size] = '\0';
	cout << content << endl;
	send(sClient, content, size + lSize + 1, 0);
	delete buf;
}
