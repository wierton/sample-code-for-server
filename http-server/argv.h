#ifndef ARGV_H
#define ARGV_H

#include <vector>
#include <sstream>
#include <utility>
#include <list>
#include <cstdint>

#include "StringRef.h"

// commandline
namespace cl {

enum OptionKind : uint8_t {
	ShortOptValue = 1,
	LongOptValue  = 2,
	BothOptValue  = (ShortOptValue | LongOptValue),
};

template<OptionKind kind>
struct OptionType {
	OptionType() = default;

	static constexpr OptionKind value = kind;
};

using ShortOptType = OptionType<ShortOptValue>;
using LongOptType  = OptionType<LongOptValue>;
using BothOptType  = OptionType<BothOptValue>;

extern ShortOptType ShortOpt;
extern LongOptType  LongOpt;
extern BothOptType  BothOpt;


class Argv {
	StringRef Desc;

	static std::vector<Argv *> &getOptions();

public:
	Argv();
	Argv(StringRef desc);

	static void parseCommandline(int argc, const char **argv);

	virtual int parseAt(const char **start, const char **end) = 0;
	virtual int weight() = 0;

	virtual ~Argv() = default;
};

template<class T>
class opt : public Argv {
	StringRef ShortKey;
	StringRef LongKey;
	T Value;
	OptionKind Kind;
	bool Inited;

private:
	int parseValue(StringRef begin, const char **start, const char **end) {
		StringRef ValueString;
		auto range = 0;
		if(begin.empty()) {
			if(start + 1 < end) {
				ValueString = *(start + 1);
				range = 2;
			} else {
				return -1;
			}
		} else {
			ValueString = begin;
			range = 1;
		}
		std::istringstream(ValueString.getData()) >> Value;
		return range;
	}

	int parseOption(StringRef prefix, StringRef infix, StringRef key,
			const char **start, const char **end) {
		auto begin = StringRef(*start);
		if(!begin.startsWith(prefix)) return 0;

		begin = begin.substr(prefix.size());
		if(!begin.startsWith(key)) return 0;
		begin = begin.substr(key.size());

		if(infix.size() && begin.size()) {
			if(begin.startsWith(infix)) {
				begin = begin.substr(infix.size());
			} else {
				return 0;
			}
		}

		auto range = parseValue(begin, start, end);
		Inited = true;
		return range;
	}

	int parseAsShortOpt(const char **start, const char **end) {
		return parseOption("-", "", ShortKey, start, end);
	}

	int parseAsLongOpt(const char **start, const char **end) {
		return parseOption("--", "=", LongKey, start, end);
	}
	
public:

	opt(ShortOptType placeholder, StringRef key) :
		ShortKey(key), LongKey(), Value(),
		Kind(ShortOptValue), Inited(false)
	{
		assert(key.size() == 1 && "short option can only has one single character as key.");
	}

	opt(LongOptType placeholder, StringRef key) :
		ShortKey(), LongKey(key), Value(),
		Kind(LongOptValue), Inited(false)
	{
	}

	opt(BothOptType placeholder, StringRef shortKey, StringRef longKey) :
		ShortKey(shortKey), LongKey(longKey), Value(),
		Kind(BothOptType::value), Inited(false)
	{
		assert(shortKey.size() == 1 && "short option can only has one single character as key.");
	}

	template<class U>
	opt(U &&) = delete;

	int parseAt(const char **start, const char **end) override {
		auto range = 0;
		if(Kind & ShortOptValue)
			range = parseAsShortOpt(start, end);

		if(range == 0 && Kind & LongOptValue)
			range = parseAsLongOpt(start, end);

		return range;
	}

	int weight() override { return 0; }

	operator bool() {
		return Inited;
	}

	bool hasValue() {
		return Inited;
	}

	const T &value() {
		return Value;
	}
};

template<>
class opt<void> : public Argv {
	StringRef ShortKey;
	StringRef LongKey;
	OptionKind Kind;
	bool Inited;

public:

	opt(ShortOptType placeholder, StringRef key) :
		ShortKey(key), LongKey(),
		Kind(ShortOptValue), Inited(false)
	{
		assert(key.size() == 1 && "short option can only has one single character as key.");
	}

	opt(LongOptType placeholder, StringRef key) :
		ShortKey(), LongKey(key),
		Kind(LongOptValue), Inited(false)
	{
	}

	opt(BothOptType placeholder, StringRef shortKey, StringRef longKey) :
		ShortKey(shortKey), LongKey(longKey),
		Kind(BothOptType::value), Inited(false)
	{
		assert(shortKey.size() == 1 && "short option can only has one single character as key.");
	}

	int parseAt(const char **start, const char **end) override {
		auto begin = StringRef(*start);

		if(Kind & LongOptValue) {
			if(begin.startsWith("--")
			&& begin.substr(2) == LongKey) {
				Inited = true;
				return 1;
			}
		}

		if(Kind & ShortOptValue) {
			if(!begin.startsWith("-")) return 0;
			if(begin.substr(1) != ShortKey) return 0;
			Inited = true;
			return 1;
		}

		return 0;
	}

	int weight() override { return 0; }

	operator bool() {
		return Inited;
	}

	bool hasValue() {
		return false;
	}
};

template<class T>
class list : public Argv {
	StringRef Desc;
	std::vector<T> Values;
	bool Inited;
public:
	list(StringRef desc) :
		Desc(desc), Values(), Inited(false)
	{
	}

	int parseAt(const char **start, const char **end) override {
		auto range = 0;
		Inited = true;
		if(StringRef(*start) == "--") {
			range ++;
			for(; start + range < end; range ++) {
				T value;
				std::istringstream(*(start + range)) >> value;
				Values.push_back(std::move(value));
			}
			return range;
		} else {
			T value;
			std::istringstream(*start) >> value;
			Values.push_back(std::move(value));
			return 1;
		}
	}

	int weight() override { return 1; }

	operator bool() {
		return Inited;
	}

	bool hasValue() {
		return Inited;
	}

	const std::vector<T> &values() {
		return Values;
	}
};

}


#endif
