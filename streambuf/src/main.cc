#include <iostream>
#include <streambuf>

class outbuf : public std::streambuf {
protected:
	int overflow (int ch) override {
		if(ch != EOF) {
			ch = std::toupper(ch);
			if(std::putchar(ch) == EOF) {
				return EOF;
			}
		}
		return ch;
	}
};

int main() {
	outbuf ob;
	std::ostream os(&ob);
	os << "abcdABCD" << std::endl;
	return 0;
}

