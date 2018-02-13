#ifndef SERVER_H
#define SERVER_H

#include <functional>
#include <map>
#include <set>
#include <string>
#include <regex>
#include <iostream>
#include <streambuf>

#include "file.h"
#include "tcpstream.h"


class TCPServer {
	int port;
	int servfd;

	static std::set<int> opened_servfds;
private:
	void init_servfd();

public:
	TCPServer(int port=80); // default to be 80 port
	~TCPServer();

	void init_servfd(int port);
	TCPStream accept_client();

	static void shutdown();
};

enum HTTPMethod { GET, POST, PUT, PATCH };

class HTTPResponse {
	int _return_code;
	std::map<std::string, std::string> _header;
	std::string _body;

	static const std::map<std::string, std::string> filetype;

	friend class HTTPServer;
public:
	HTTPResponse();
	HTTPResponse(File &fp);
	HTTPResponse(std::string &&body);
	HTTPResponse(const char *body);
	HTTPResponse(std::map<std::string, std::string> &&header, std::string &&body);

	friend std::ostream &operator<<(std::ostream &os, const HTTPResponse &response);
};

class HTTPRequest {
	HTTPMethod _method;
	std::string _path;
	std::map<std::string, std::string> _get;
	std::map<std::string, std::string> _post;
	std::map<std::string, std::string> _header;
	std::map<std::string, std::string> _cookie;
	std::string _body;

	const std::string novalue;

	std::istream &iss;
private:

	void parse_method();
	void parse_path();
	void parse_get_arguments();
	void parse_version();
	void parse_firstline();
	void parse_header_oneline();
	bool check_if_end_of_header();
	void parse_header();
	void parse_body();

	void parse_cookie();

public:
	HTTPRequest(std::istream &iss);

	HTTPMethod method();
	const std::string &path();
	const std::string &get(const std::string &key);
	const std::string &post(const std::string &key);
	const std::string &header(const std::string &key);
	const std::string &cookie(const std::string &key);
};

using Session = decltype(0); // hasn't been implemented

using CallbackArgs = std::vector<std::string>;
class Callback {
	using callback_t = std::function<HTTPResponse (Session &, CallbackArgs &)>;

	std::regex group_pattern;
	callback_t callback;
	std::vector<std::string> args;
public:
	Callback(const std::string &key, const callback_t &callback);

	bool init(const std::string &path);
	HTTPResponse operator()(Session &session);
};


class SignalHandler {
	static bool _int_bit;
public:
	static void sigint_handler(int);
	static void register_sighandler();
};

class HTTPServer : private TCPServer {
	// session ID
	std::map<std::string, Session> sessions;
	std::vector<Callback> callbacks;


private:
	Callback &find_callback(const std::string &path);

public:
	HTTPServer(int port=80);

	static void shutdown();

	void config(const std::string &filename);
	// void config(Json _config);

	void register_callback(const Callback &cb);
	void register_callbacks(const std::vector<Callback> &cbs);
	void run();
};


#endif
