#pragma once

#include <iostream>
#include <istream>
#include <ostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>


using namespace std;
using namespace chrono;


void test0(string arg) {
	cout << arg << std::endl;
}

void test1() {
	cout << "wait for 500ms" << endl;
	this_thread::sleep_for(milliseconds(500));
	cout << "Hello C++11" << endl;
	cout << "wait for 1000ms" << endl;
	this_thread::sleep_until(system_clock::now() + milliseconds(1000));
	cout << "wake up" << endl;
	cout<<"id:" << this_thread::get_id() << endl;
}

void test2() {
}

void test() {
	thread thread(test1);
	thread.join();
}

