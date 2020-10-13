#include "webserver.h"
#include "http/http_conn.h"
#include "config.h"
#include "helper.h"
#include "tester/test_string.hpp"
#include <direct.h>

using namespace std;

#pragma comment(lib,"ws2_32.lib") 

int main(){
	Config config("web.conf");
	//initConf(config);
	Http http(config);
	bool res=http.init();
	if (res) {
		http.loop();
	}
	else {
		setColor(RED);
		cout << "fail to initialize" << endl;
	}

	return 0;
}