#include "webserver.h"
#include "http/http_conn.h"
#include "config.h"
#include "helper.h"
#include "tester/test_string.hpp"

//#include <stdio.h>  
//#include <fstream>
//#include <istream>
//#include <iostream>
#include <string>
//#include <winsock2.h>  

using namespace std;

#pragma comment(lib,"ws2_32.lib") 




int main(){
	Config config;
	/*
	设置config
	//*/
	Http http(config);
	//http.init();
	//http.loop();	
	http.getMappings("E:\\DESKTOP\\VSproject\\Socket\\x64\\Debug\\web.conf");

	return 0;
}