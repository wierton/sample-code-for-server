#include "helper.h"
#include "debug.h"
#include <sstream>
#include <iostream>
#include <cassert>
#include <string>
using namespace std;

AUTO_TEST(test_ignore_until_n) {
	istringstream iss("123 456 789");
	string out;
	ignore_until(iss, " "_n);
	std::getline(iss, out);
	assert(out == " 456 789");
}

AUTO_TEST(test_ignore_while_n) {
	istringstream iss("123 456 789");
	string out;
	ignore_while(iss, "123456789"_n);
	std::getline(iss, out);
	assert(out == " 456 789");
}

AUTO_TEST(test_peek_while_n) {
	istringstream iss("123 456 789");
	string out;
	peek_while(iss, out, "123456789"_n);
	assert(out == "123");
}

AUTO_TEST(test_peek_until_n) {
	istringstream iss("123 456 789");
	string out;
	peek_until(iss, out, " "_n);
	assert(out == "123");
}

AUTO_TEST(test_ignore_until) {
	istringstream iss("123 456 789");
	string out;
	auto matcher = [](char ch) { return std::isspace(ch); };
	ignore_until(iss, matcher);
	std::getline(iss, out);
	assert(out == " 456 789");
}

AUTO_TEST(test_ignore_while) {
	istringstream iss("123 456 789");
	string out;
	auto matcher = [](char ch) { return std::isdigit(ch); };
	ignore_while(iss, matcher);
	std::getline(iss, out);
	assert(out == " 456 789");
}

AUTO_TEST(test_peek_while) {
	istringstream iss("123 456 789");
	string out;
	auto matcher = [](char ch) { return std::isdigit(ch); };
	peek_while(iss, out, matcher);
	assert(out == "123");
}

AUTO_TEST(test_strip) {
	std::string s = "  asd  ";
	strip(s);
	assert(s == "asd");
}

