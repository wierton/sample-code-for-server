#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>

#include <map>
#include <cctype>
#include <algorithm>
#include <memory>

#include "debug.h"
#include "server.h"
#include "helper.h"

std::set<int> TCPServer::opened_servfds;

const std::map<std::string, std::string> HTTPResponse::filetype = {
	{"html","text/html"},
	{"css" ,"text/css"},
	{"js"  ,"application/js"},
	{"json","application/json"},
	{"jpg" ,"image/jpg"},
	{"jpeg","image/jpeg"},
	{"png" ,"image/png"},
	{"ico" ,"image/ico"},
	{"gif" ,"image/gif"},
};


TCPServer::TCPServer(int port) :
	port(port),
	servfd(-1)
{
	init_servfd();
}


TCPServer::~TCPServer() {
	if(servfd > 0) {
		wlog("close server fd %\n", servfd);
		opened_servfds.erase(servfd);
		close(servfd);
	}
}

void TCPServer::init_servfd() {
	servfd = socket(AF_INET, SOCK_STREAM, 0);
	if(servfd < 0) {
		wloge("Create Server Socket Failed!\n");
	}

	// set REUSEADDR
	int flag = 1;
	setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag));

	// create sockaddr
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(servfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
		wloge("Can not bind to port %!\n", port);
	}

	// FIXME:
	if(listen(servfd, 50) == -1) {
		wloge("fail to listen on socket.\n");
	}

	opened_servfds.insert(servfd);
}


void TCPServer::init_servfd(int port) {
	// re-init
	this->~TCPServer();
	this->port = port;
	init_servfd();
}

void TCPServer::shutdown() {
	for(auto fd : opened_servfds) {
		wlog("close fd %\n", fd);
		close(fd);
	}

	opened_servfds.clear();
}

TCPStream TCPServer::accept_client() {
	struct sockaddr_in client_addr;
	socklen_t length = sizeof(client_addr);
	int conn = accept(servfd, (struct sockaddr*)&client_addr, &length);
	wlog("connected by %:%, conn:%\n", inet_ntoa(client_addr.sin_addr), client_addr.sin_port, conn);
	if(conn < 0) {
		wloge("fail to accept client.\n");
	}

	return TCPStream(conn);
}

HTTPRequest::HTTPRequest(std::istream &iss) :
	_method(GET),
	_path(),
	_get(),
	_post(),
	_header(),
	_cookie(),
	_body(),
	novalue(),
	iss(iss)
{
	parse_firstline();
	parse_header();
	parse_body();
}

void HTTPRequest::parse_method() {
	std::string method_s;
	peek_word(iss, method_s);
	for(auto &ch : method_s) ch = std::toupper(ch);

	static std::map<std::string, HTTPMethod> conversion {
		{"GET", GET},
		{"POST", POST},
	};
	_method = conversion[method_s];

	wlog("method:%-->%\n", method_s, _method);
}

void HTTPRequest::parse_path() {
	ignore_blank(iss);
	peek_until(iss, _path, " \t\r\n?"_n);
	wlog("path before:'%'\n", _path);
	if(_path[0] == '/')
		_path.erase(0, 1);

	wlog("path:'%'\n", _path);
}

void HTTPRequest::parse_get_arguments() {
	if(iss.peek() != '?') return;
	iss.ignore();

	while(iss.good()) {
		std::string key, value;
		peek_until(iss, key, "=& \t\r\n"_n);
		if(std::isspace(iss.peek()))
			break;

		if(iss.peek() == '=') {
			iss.ignore();
			peek_until(iss, value, "& \t\r\n"_n);
		}

		if(iss.peek() == '&')
			iss.ignore();

		wlog("GET[%]=%\n", key, value);
		_get[std::move(key)] = value;

		if(std::isspace(iss.peek()))
			break;
	}
}

void HTTPRequest::parse_version() {
	ignore_until(iss, "\r\n"_n);
	ignore_while(iss, "\r\n"_n);
}

void HTTPRequest::parse_firstline() {
	parse_method();
	parse_path();
	parse_get_arguments();
	parse_version();
}

void HTTPRequest::parse_header_oneline() {
	while(iss.good()) {
		std::string key, value;
		peek_until(iss, key, ":\r\n"_n);

		strip(key); // erase spaces

		if(isspace(iss.peek()))
			break;

		if(iss.peek() == ':') {
			iss.ignore();
			ignore_while(iss, " \t"_n);
			peek_until(iss, value, "\r\n"_n);
		}

		wlog("HEADER[%]:%\n", key, value);
		_header[std::move(key)] = value;

		if(isspace(iss.peek()))
			break;
	}
}

bool HTTPRequest::check_if_end_of_header() {
	// \r\r \n\n \r\n\r\n
	goto st;

	st:{
		char ch = iss.peek();
		if(ch == '\r') {
			iss.ignore();
			goto r1;
		} else if(ch == '\n') {
			iss.ignore();
			goto n1;
		} else {
			return false;
		}
	}

	r1:{
		char ch = iss.peek();
		if(ch == '\r') {
			iss.ignore();
			return true;
		} else if(ch == '\n') {
			iss.ignore();
			goto r1n1;
		} else {
			return false;
		}
	}

	n1:{
		char ch = iss.peek();
		if(ch == '\n') {
			iss.ignore();
			return true;
		} else {
			return false;
		}
	}
	
	r1n1:{
		char ch = iss.peek();
		if(ch == '\r') {
			iss.ignore();
			goto r1n1r1;
		} else {
			return false;
		}
	}

	r1n1r1:{
		char ch = iss.peek();
		if(ch == '\n') {
			iss.ignore();
			return true;
		} else {
			iss.unget();
			return false;
		}
	}
}

void HTTPRequest::parse_header() {
	ignore_while(iss, "\r\n"_n);
	while(iss.good()) {
		wlog("parse header oneline\n");
		parse_header_oneline();
		if(!check_if_end_of_header()) {
			wlog("not end of header\n");
			ignore_while(iss, "\r\n"_n);
		} else {
			wlog("end of header\n");
			break;
		}
	}
}

void HTTPRequest::parse_cookie() {
}

void HTTPRequest::parse_body() {
	size_t length = 0;
	auto key = get("Content-Length");
	wlog("no key Content-Length\n");
	if(key == "") return;

	std::istringstream(key) >> length;
	wlog("content-length:%->%\n", key, length);
	while(length && iss.good()) {
		_body.push_back(iss.get());
		length --;
	}
}

HTTPMethod HTTPRequest::method() {
	return _method;
}

const std::string &HTTPRequest::path() {
	return _path;
}

const std::string &HTTPRequest::get(const std::string &key) {
	auto it = _get.find(key);
	return it == _get.end() ? novalue : it->second;
}

const std::string &HTTPRequest::post(const std::string &key) {
	auto it = _post.find(key);
	return it == _post.end() ? novalue : it->second;
}

const std::string &HTTPRequest::header(const std::string &key) {
	auto it = _header.find(key);
	return it == _header.end() ? novalue : it->second;
}


std::ostream &operator<<(std::ostream &os, const HTTPResponse &response) {
	os << "HTTP/1.1 200 OK\r\n";

	for(auto &kvpair : response._header)
		os << kvpair.first << ": " << kvpair.second << "\r\n";

	os << "\r\n";
	os << response._body;
	return os;
}

Callback::Callback(const std::string &key, const callback_t &callback) :
	group_pattern(key),
	callback(callback),
	args()
{
}

bool Callback::init(const std::string &path) {
	std::smatch match_results;
	wlog("init with path '%'\n", path);
	std::regex_match(path, match_results, group_pattern);
	if(match_results.empty()) return false;

	wlog("extract match results\n");
	args.clear();
	for(auto &sub : match_results)
		args.push_back(sub);
	return true;
}

HTTPResponse Callback::operator()(Session &session) {
	return callback(session, args);
}


///   signal handler
void SignalHandler::sigint_handler(int signum) {
	wlog("receive keyboard interrupt\n");
	HTTPServer::shutdown();
	exit(0);
}

void SignalHandler::register_sighandler() {
	signal(SIGINT, sigint_handler);
}


HTTPServer::HTTPServer(int port) :
	TCPServer(port),
	sessions(),
	callbacks()
{
	auto default_callback = [](Session &session, CallbackArgs &args) -> HTTPResponse {
		return "<html> 404 </html>";
	};

	register_callback({R"((|.*))", default_callback});
}

void HTTPServer::shutdown() {
	TCPServer::shutdown();
}

void HTTPServer::register_callback(const Callback &cb) {
	callbacks.push_back(cb);
}

void HTTPServer::register_callbacks(const std::vector<Callback> &cbs) {
	for(auto &cb : cbs)
		callbacks.push_back(cb);
}

Callback &HTTPServer::find_callback(const std::string &path) {
	wlog("callback at path '%'\n", path);

	auto real_path = path;

	if(real_path.size() == 0 || File(real_path).is_directory()) {
		real_path += "/index.html";
	}

	for(auto it = callbacks.rbegin();
			it != callbacks.rend();
			++it) {
		if(it->init(real_path)) {
			return *it;
		}
	}
	wlog("callback not found, return default callback\n");
	return callbacks.front();
}

HTTPResponse::HTTPResponse() :
	_return_code(200),
	_header(),
	_body()
{
}

HTTPResponse::HTTPResponse(File &fp) :
	_return_code(200),
	_header(),
	_body()
{
	auto suffix = fp.file_suffix();
	_body = fp.readall();

	auto it = filetype.find(suffix);
	if(it != filetype.end())
		_header["Content-Type"] = it->second;
	
	_header["Content-Length"] = std::to_string(_body.size());
}

HTTPResponse::HTTPResponse(const char *body) :
	_return_code(200),
	_header(),
	_body(body)
{
	_header["Content-Length"] = std::to_string(_body.size());
}

HTTPResponse::HTTPResponse(std::string &&body) :
	_return_code(200),
	_header(),
	_body(std::move(body))
{
	_header["Content-Length"] = std::to_string(_body.size());
}

HTTPResponse::HTTPResponse(std::map<std::string, std::string> &&header, std::string &&body) :
	_return_code(200),
	_header(),
	_body(std::move(body))
{
	for(auto &kvpair : header)
		_header[std::move(kvpair.first)] = std::move(kvpair.second);
	_header["Content-Length"] = std::to_string(_body.size());
}

void HTTPServer::config(const std::string &filename) {
}

void HTTPServer::run() {
	SignalHandler::register_sighandler();
	while(1) {
		auto client = accept_client();
		HTTPRequest request(client);
		auto &callback = find_callback(request.path());
		auto response = callback(sessions[""]);
		client << response;
	}
	wlog("close http server\n");
}

