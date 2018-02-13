#ifndef STRINGREF_H
#define STRINGREF_H

#include <tuple>
#include <string>
#include <cassert>
#include <cstring>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include <functional>


// wrapper for string literal
class StringRef {
	const char *data;
	size_t length;

    static int compareMemory(const char *lhs, const char *rhs, size_t length) {
      if (length == 0) { return 0; }
      return ::memcmp(lhs, rhs, length);
    }

public:
	static constexpr size_t npos = ~size_t(0);
	using iterator = const char *;
	using const_iterator = const char *;
	using size_type = size_t;

	StringRef() :
		data(""), length(0)
	{
	}

	StringRef(const char *s) :
		data(s), length(s ? ::strlen(s) : 0)
	{
	}

	StringRef(const char *begin, size_t length) :
		data(begin), length(length)
	{
	}

	StringRef(const StringRef &that) = default;
	StringRef(StringRef &&that) = default;

	StringRef& operator=(const StringRef &that) = default;
	StringRef& operator=(StringRef &&that) = default;

	const char *getData() const { return data; }

	char operator[](const size_t idx) const {
		return data[idx];
	}

	char at(const size_t idx) const {
		if(idx > length) {
			throw std::out_of_range("function at: index out of range");
		}
		return data[idx];
	}

	iterator begin() const { return data; }
	iterator end()   const { return data + length; }

	bool empty() const {
		return !(data && length);
	}

	size_t size() {
		return length;
	}

	char front() const {
		assert(!empty() && "take front from a null string");
		return *data;
	}

	char back() const {
		assert(!empty() && "take back from a null string");
		return data[length - 1];
	}

    std::string str() const {
      if (!data) return std::string();
      return std::string(data, length);
    }

	size_t find_if(std::function<bool(char)> F) const {
		for(auto i = 0u; i < length; i++) {
			if(F(data[i]))
				return i;
		}
		return npos;
	}

	size_t find_if_not(std::function<bool(char)> F) const {
		for(auto i = 0u; i < length; i++) {
			if(!F(data[i]))
				return i;
		}
		return npos;
	}

	size_t find_first_of(char ch) const {
		for(auto i = 0u; i < length; i++) {
			if(data[i] == ch)
				return i;
		}
		return npos;
	}

	size_t find_first_not_of(char ch) const {
		for(auto i = 0u; i < length; i++) {
			if(data[i] != ch)
				return i;
		}
		return npos;
	}

	size_t find_last_of(char ch) const {
		for(int i = length - 1; i >= 0; i--) {
			if(data[i] == ch)
				return i;
		}
		return npos;
	}

	size_t find_last_not_of(char ch) const {
		for(int i = length - 1; i >= 0; i--) {
			if(data[i] != ch)
				return i;
		}
		return npos;
	}

    StringRef substr(size_t start, size_t N = npos) const {
      start = std::min(start, length);
      return StringRef(data + start, std::min(N, length - start));
    }

	std::pair<StringRef, StringRef> split(char ch) const {
		auto pos = find_first_of(ch);
		if(pos == npos)
			return std::make_pair(*this, StringRef());
		return { substr(0, pos), substr(pos) };
	}

	//
    bool startsWith(StringRef Prefix) const {
      return length >= Prefix.length &&
		compareMemory(data, Prefix.data, Prefix.length) == 0;
    }

    bool endsWith(StringRef Suffix) const {
      return length >= Suffix.length &&
		compareMemory(end() - Suffix.length, Suffix.data, Suffix.length) == 0;
    }

	bool getAsInteger() {
		return 0;
	}

    bool equals(StringRef rhs) const {
      return (length == rhs.length &&
              compareMemory(data, rhs.data, rhs.length) == 0);
    }

    int compare(StringRef rhs) const {
      if (int ret = compareMemory(data, rhs.data, std::min(length, rhs.length)))
        return ret < 0 ? -1 : 1;

      if (length == rhs.length)
        return 0;
      return length < rhs.length ? -1 : 1;
    }

	operator std::string() {
		return std::string(data, data + length);
	}
};

inline bool operator==(StringRef lhs, StringRef rhs) {
	return lhs.equals(rhs);
}

inline bool operator!=(StringRef lhs, StringRef rhs) {
	return !(lhs == rhs);
}

inline bool operator<(StringRef lhs, StringRef rhs) {
	return lhs.compare(rhs) == -1;
}

inline bool operator<=(StringRef lhs, StringRef rhs) {
	return lhs.compare(rhs) != 1;
}

inline bool operator>(StringRef lhs, StringRef rhs) {
	return lhs.compare(rhs) == 1;
}

inline bool operator>=(StringRef lhs, StringRef rhs) {
	return lhs.compare(rhs) != -1;
}

inline std::string &operator+=(std::string &buffer, StringRef string) {
	return buffer.append(string.getData(), string.size());
}

inline std::ostream &operator<<(std::ostream &os, StringRef rhs) {
	for(char ch : rhs)
		os << ch;
	return os;
}

#endif
