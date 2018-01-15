#include "server.h"
#include "file.h"
#include "debug.h"

#include <cassert>

HTTPResponse add(Session &session, CallbackArgs &args) {
	std::ostringstream oss;
	assert(args.size() == 3);
	oss << args[1] << " + " << args[2] << " = " << atoi(args[1].c_str()) + atoi(args[2].c_str()) << std::endl;
	return oss.str();
}

HTTPResponse file(Session &session, CallbackArgs &args) {
	wlog("here???\n");
	auto fp = File(args[0]);

	if(!fp.is_exists())
		return "<html> 404 </html>";

	if(fp.is_directory())
		return "";

	return fp;
}

HTTPResponse h404(Session &session, CallbackArgs &args) {
	return "<html> 404 </html>";
}


int main(int argc, char **argv) {
	HTTPServer server(8080);
	server.register_callback({R"(.*)", file});
	server.register_callback({R"(add/(\d+)/(\d+))", add});
	server.run();
	return 0;
}
