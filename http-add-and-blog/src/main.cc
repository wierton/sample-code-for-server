#include "server.h"
#include "file.h"
#include "debug.h"

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
	wlog("here???\n");
	auto fp = File(work_directory + args[0]);

	if(!fp.is_exists())
		return "<html> 404 </html>";

	if(fp.is_directory())
		return "";

	return fp;
}


/* @param(1)
 *	  argument count
 * @param(2)
 *    argv[1]: work directory
 *
 */
int main(int argc, char **argv) {
	HTTPServer server(8080);

	if(argc <= 1) {
		std::cout << "[ERROR] please specify work directory\n";
		std::cout << "    eg. <program> ./dir\n";
		exit(0);
	}

	work_directory = std::string(argv[1]) + "/";

	server.register_callback({R"(.*)", file});
	server.register_callback({R"(add/(\d+)/(\d+))", add});
	server.run();
	return 0;
}



