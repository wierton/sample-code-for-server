#include "argv.h"
#include <algorithm>

namespace cl {

ShortOptType ShortOpt;
LongOptType  LongOpt;
BothOptType  BothOpt;


std::vector<Argv *> &Argv::getOptions() {
	static std::vector<Argv *> options;
	return options;
}

Argv::Argv(StringRef desc) :
	Desc(desc)
{
	getOptions().push_back(this);
}

Argv::Argv() :
	Argv(StringRef(""))
{
}

void Argv::parseCommandline(int argc, const char **argv) {
	std::sort(getOptions().begin(), getOptions().end(), [](Argv *lhs, Argv *rhs) {
		return lhs->weight() < rhs->weight();
	});

	for(int i = 1; i < argc;) {
		auto range = 0;
		for(auto option : getOptions()) {
			range = option->parseAt(argv + i, argv + argc);
			if(range) break;
		}

		if(range > 0) {
			i += range;
			continue;
		} else if(range == 0) {
			std::clog << "Unrecognized option at "
				<< i << ": '" << argv[i] << "'\n";
			exit(0);
		} else {
			std::clog << "Expected specifier after "
				<< i << ": '" << argv[i] << "'\n";
			exit(0);
		}
	}
}

}
