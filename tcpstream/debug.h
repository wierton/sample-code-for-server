#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <sstream>
#include <assert.h>
#include <stdlib.h>

/* reset terminal style and color */
#define VT_RESET           "\033[0m"

/*output style*/
#define VT_STYLE_BOLD      "\033[1m"
#define VT_STYLE_DARK      "\033[2m"
#define VT_STYLE_BACK      "\033[3m"
#define VT_STYLE_UNDERLINE "\033[4m"

/* background color */
#define VT_BG_BLACK     "\033[40m"
#define VT_BG_RED       "\033[41m"
#define VT_BG_GREEN     "\033[42m"
#define VT_BG_YELLOW    "\033[43m"
#define VT_BG_BLUE      "\033[44m"
#define VT_BG_PURPLE    "\033[45m"
#define VT_BG_SKYBLUE   "\033[46m"
#define VT_BG_WHITE     "\033[47m"
#define VT_BG_NORMAL    "\033[48m"

/*output color*/
#define VT_COLOR_BLACK     "\033[30m"
#define VT_COLOR_RED       "\033[31m"
#define VT_COLOR_GREEN     "\033[32m"
#define VT_COLOR_YELLOW    "\033[33m"
#define VT_COLOR_BLUE      "\033[34m"
#define VT_COLOR_PURPLE    "\033[35m"
#define VT_COLOR_SKYBLUE   "\033[36m"
#define VT_COLOR_WHITE     "\033[37m"
#define VT_COLOR_NORMAL    "\033[38m"


#define ERROR_TAG    VT_COLOR_RED    "[ERROR] "    VT_RESET
#define WARNING_TAG  VT_COLOR_YELLOW "[WARNING] "  VT_RESET
#define DEBUG_TAG    VT_COLOR_BLUE   "[DEBUG] "    VT_RESET


namespace wt {

inline void ostream_print(std::ostream &os, const char * fmt) {
	while(*fmt) {
		if(*fmt == '%')
			fmt ++;
		os << *fmt;
		fmt ++;
	}
}

template<class T, class... Args>
void ostream_print(std::ostream &os, const char * fmt, T firstArg, Args... restArgs) {
	while(*fmt) {
		if(*fmt == '%') {
			fmt ++;
			if(*fmt == '%') {
				os << '%';
				fmt ++;
				continue;
			} else {
				os << firstArg;
				ostream_print(os, fmt, restArgs...);
				return;
			}
		}
		os << *fmt;
		fmt ++;
	}
}

template<class... Args>
inline std::size_t sprint(std::string &sout, const char *fmt, Args... args) {
	std::ostringstream os;
	ostream_print(os, fmt, args...);
	sout = os.str();
	return sizeof...(Args);
}

template<class... Args>
inline std::string sformat(const char *fmt, Args... args) {
	std::ostringstream os;
	ostream_print(os, fmt, args...);
	return os.str();
}

}



/* wt's debug */
namespace wtd {

template<class... Args>
void log_r(const char *fmt, Args... args) {
	wt::ostream_print(std::cerr, fmt, args...);
}

template<class... Args>
size_t tlog(const char *tag, const char *file, const char *func, const int line, const char *fmt, Args... args) {
	std::clog << tag << file << ": " << func << ": " << line << ": ";
	log_r(fmt, args...);
	return sizeof...(Args);
}

} // end of namespace wt

#define wlog(...) wtd::tlog(DEBUG_TAG, __FILE__, __func__, __LINE__, __VA_ARGS__)

#define wloge(...) do {	\
	wtd::tlog(ERROR_TAG, __FILE__, __func__, __LINE__, __VA_ARGS__);	\
	abort();			\
} while(0)

#define dynamic_abort(...) loge(__VA_ARGS__)

#define dynamic_assert(cond, ...) do {	\
	if(!(cond)) {						\
		loge("" __VA_ARGS__);			\
	}									\
} while(0)


#define AUTO_TEST(F)					\
	namespace wt_test {					\
		void F();						\
		int result_of_##F = (F(), 0);	\
	}									\
	void wt_test::F()

#define AUTO_RUN(F)						\
	namespace wt_run {					\
		void F();						\
		int result_of_##F = (F(), 0);	\
	}									\
	void wt_run::F()

#endif
