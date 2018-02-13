#include "server.h"
#include "file.h"
#include "debug.h"

#include "argv.h"

#include <string>
#include <cassert>

std::string work_directory; // bad solution

HTTPResponse add(Session &session, CallbackArgs &args) {
	std::ostringstream oss;
	assert(args.size() == 3);
	oss << args[1] << " + " << args[2] << " = " << atoi(args[1].c_str()) + atoi(args[2].c_str()) << std::endl;
	return oss.str();
}

HTTPResponse file(Session &session, CallbackArgs &args) {
	auto fp = File(work_directory + args[0]);

	if(!fp.is_exists())
		return "<html> 404 </html>";

	if(fp.is_directory())
		return "";

	return fp;
}


static cl::opt<std::string> WorkDirectory(cl::BothOpt, "w", "work-directory");
static cl::opt<int> Port(cl::BothOpt, "p", "port");
static cl::opt<void> Help(cl::BothOpt, "h", "help");

/* @param(1)
 *	  argument count
 * @param(2)
 *    argv[1]: work directory
 *
 */
int main(int argc, const char **argv) {
	cl::Argv::parseCommandline(argc, argv);

	if(Help) {
		std::clog << "usage:\n";
		std::clog << "<bin> -p {port}/--port={port}\n";
		std::clog << "<bin> -w {dir}/--work-directory={dir}\n";
		std::clog << "\n";
		return 0;
	}

	// default option
	auto port = Port ? Port.value() : 8080;
	work_directory = WorkDirectory ? WorkDirectory.value() + "/" : "./";

	// run server
	HTTPServer server(port);
	server.register_callback({R"(.*)", file});
	server.register_callback({R"(add/(\d+)/(\d+))", add});
	server.run();
	return 0;
}



