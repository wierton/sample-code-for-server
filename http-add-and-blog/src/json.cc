#include "json.h"

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <cstdint>
#include <sstream>
#include <cassert>
#include <utility>
#include <stdexcept>
#include <initializer_list>

Json Json::null;

class JsonElement {
public:
	virtual Json::Type type() const = 0;
	
	bool is_null()    const { return type() == Json::NIL; }
	bool is_boolean() const { return type() == Json::BOOLEAN; }
	bool is_number()  const { return type() == Json::NUMBER; }
	bool is_string()  const { return type() == Json::STRING; }
	bool is_array()   const { return type() == Json::ARRAY; }
	bool is_object()  const { return type() == Json::OBJECT; }

	virtual bool is_integer() const { return false; }
	virtual bool is_float()   const { return false; }

	virtual bool to_boolean()       const { assert(0); }
	virtual int64_t to_integer()    const { assert(0); }
	virtual double to_float()       const { assert(0); }
	virtual std::string to_string() const { assert(0); }

	virtual const std::string &as_string()  const { assert(0); }
	virtual const Json::Array &as_array()   const { assert(0); }
	virtual const Json::Object &as_object() const { assert(0); }

	virtual size_t size()           const { assert(0); }
	virtual Json &at(const std::string &) { assert(0); }
	virtual Json &at(const size_t idx)    { assert(0); }

	virtual Json clone() = 0;

	virtual bool equals_to(const Json &) const = 0;

	virtual ~JsonElement() = default;
};

class JNull : public JsonElement {
public:
	JNull() = default;
	JNull(nullptr_t) { }
	Json::Type type() const override { return Json::NIL; }

	Json clone() { return Json(nullptr); }

	bool equals_to(const Json &json) const override {
		return json.is_null();
	}

	std::string to_string() const override {
		return std::string("null");
	}
};

class JBoolean : public JsonElement {
	bool b;
public:
	JBoolean(bool b) : b(b) { }
	Json::Type type() const override { return Json::BOOLEAN; }
	bool to_boolean() const override { return b; }

	Json clone() { return Json(b); }

	bool equals_to(const Json &json) const override {
		return json.is_boolean() && json.to_boolean() == b;
	}

	std::string to_string() const override {
		return b ? std::string("true") : "false";
	}
};

class JNumber : public JsonElement {
public:
	Json::Type type() const override { return Json::NUMBER; }
	virtual ~JNumber() = default;
};

class JInteger : public JNumber {
	int64_t i;
public:
	JInteger(int64_t i) : i(i) { }
	bool is_integer()    const override { return true; }
	int64_t to_integer() const override { return i; }
	double to_float()    const override { return i; }

	Json clone() { return Json(i); }

	bool equals_to(const Json &json) const override {
		return json.is_integer() && json.to_integer() == i;
	}

	std::string to_string() const override {
		return std::to_string(i);
	}
};

class JFloat : public JNumber {
	double f;
public:
	JFloat(double f) : f(f) { }
	bool is_float()      const override { return true; }
	int64_t to_integer() const override { return f; }
	double to_float()    const override { return f; }

	Json clone() { return Json(f); }

	bool equals_to(const Json &json) const override {
		return json.is_float() && json.to_float() == f;
	}

	std::string to_string() const override {
		return std::to_string(f);
	}
};

class JString : public JsonElement {
	std::string s;
public:
	JString(const std::string &s) : s(s) { }
	JString(std::string &&s) : s(std::move(s)) { }
	Json::Type type() const override { return Json::STRING; }

	Json clone() { return Json(s); }

	bool equals_to(const Json &json) const override {
		return json.is_string() && json.as_string() == s;
	}

	const std::string &as_string() const override {
		return s;
	}

	std::string to_string() const override {
		return "\"" + s + "\"";
	}
};

class JArray : public JsonElement {
	Json::Array array;
public:
	JArray(const std::vector<Json> &array) : array(array) { }
	JArray(std::vector<Json> &&array) : array(std::move(array)) { }
	JArray(std::initializer_list<Json> && ilist) : array(ilist) { }
	Json::Type type() const override { return Json::ARRAY; }
	size_t size() const override { return array.size(); }

	const Json::Array &as_array() const override { return array; }

	Json clone() {
		Json::Array clone_array;
		for(auto &e : array) {
			clone_array.push_back(e.clone());
		}
		return Json(std::move(clone_array));
	}

	bool equals_to(const Json &json) const override {
		if(!json.is_array()) return false;
		return array == json.as_array();
	}

	std::string to_string() const override {
		std::ostringstream oss;
		oss << "[";

		auto it = array.begin();
		if(it != array.end()) {
			oss << it->to_string();
			it ++;
		}

		for(;it != array.end(); it ++) {
			oss << ", " << it->to_string();
		}
		oss << "]";
		return oss.str();
	}

	Json &at(const size_t idx) override {
		if(idx > array.size())
			array.resize(idx + 1);
		return array.at(idx);
	}
};

class JObject : public JsonElement {
	Json::Object object;
public:
	JObject(const Json::Object &object) : object(object) { }
	JObject(Json::Object &&object) : object(std::move(object)) { }
	Json::Type type() const override { return Json::OBJECT; }

	const Json::Object &as_object() const override { return object; }

	Json clone() {
		Json::Object clone_object;
		for(auto &kv : object) {
			clone_object[kv.first] = kv.second.clone();
		}
		return Json(std::move(clone_object));
	}

	bool equals_to(const Json &json) const override {
		if(!json.is_object()) return false;
		return object == json.as_object();
	}

	std::string to_string() const override {
		std::ostringstream oss;
		oss << "{";

		auto it = object.begin();
		if(it != object.end()) {
			oss << '"' << it->first << "\":" << it->second.to_string();
			it ++;
		}

		for(;it != object.end(); it ++) {
			oss << ", \"" << it->first << "\":" << it->second.to_string();
		}
		oss << "}";
		return oss.str();
	}

	Json &at(const std::string &key) override {
		return object[key];
	}
};



// Json
Json::Json() :
	je(new JNull())
{}

Json::Json(nullptr_t) :
	je(new JNull())
{}

Json::Json(bool b) :
	je(new JBoolean(b))
{}

Json::Json(int i) :
	je(new JInteger(i))
{}

Json::Json(int64_t i) :
	je(new JInteger(i))
{}

Json::Json(double f) :
	je(new JFloat(f))
{}

Json::Json(const char *s) :
	je(new JString(s))
{}

Json::Json(const std::string &s) :
	je(new JString(s))
{}

Json::Json(std::string &&s) :
	je(new JString(std::move(s)))
{}

Json::Json(const Array &array) :
	je(new JArray(array))
{}

Json::Json(Array &&array) :
	je(new JArray(std::move(array)))
{}

Json::Json(const Object &obj) :
	je(new JObject(obj))
{}

Json::Json(Object &&obj) :
	je(new JObject(std::move(obj)))
{}

Json::Json(std::initializer_list<Json> && ilist) :
	je(new JArray(ilist))
{}

Json::Json(Json && other) :
	je(other.je)
{
}

Json &Json::operator= (Json && other) {
	je = other.je;
	return *this;
}

bool Json::is_null() const    { return je->is_null(); }
bool Json::is_boolean() const { return je->is_boolean(); }
bool Json::is_number() const  { return je->is_number(); }
bool Json::is_integer() const { return je->is_integer(); }
bool Json::is_float() const   { return je->is_float(); }
bool Json::is_string() const  { return je->is_string(); }
bool Json::is_array() const   { return je->is_array(); }
bool Json::is_object() const  { return je->is_object(); }

bool Json::to_boolean() const       { return je->to_boolean(); }
int64_t Json::to_integer() const    { return je->to_integer(); }
double Json::to_float() const       { return je->to_float(); }
std::string Json::to_string() const { return je->to_string(); }

const std::string &Json::as_string() const  { return je->as_string(); }
const Json::Array &Json::as_array() const   { return je->as_array(); }
const Json::Object &Json::as_object() const { return je->as_object(); }

Json Json::clone() { return je->clone(); }

size_t Json::size() const              { return je->size(); }

Json &Json::operator[](const size_t idx)
{
	return je->at(idx);
}

Json &Json::operator[](const std::string &s)
{
	if(is_null()) *this = Json::Object{};
	return je->at(s);
}

bool operator== (const Json &a, const Json &b) {
	return a.je->equals_to(b);
}

bool operator!= (const Json &a, const Json &b) {
	return !(a == b);
}

std::ostream &operator <<(std::ostream &os, const Json &json) {
	os << json.to_string();
	return os;
}

class IterStringStream : public std::istream {
	using const_iterator = decltype(std::declval<const std::string>().begin());

	bool goodbit;
	const_iterator it, end;
public:
	IterStringStream(const std::string & s) :
		goodbit(true), it(s.begin()), end(s.end())
	{ }

	template<class T>
	IterStringStream(T&&) = delete;

	int get() {
		goodbit = it < end;
		if(!goodbit) return -1;
		char ch = *it ++;
		return ch;
	}

	int peek() {
		goodbit = it < end;
		if(!goodbit) return -1;
		return *it;
	}

	void unget() { it --; }
	void ignore() { goodbit = it < end; it ++; }
	bool good() { return goodbit; }
};

/* parser start */
class JParser {
public:
	static inline bool isEof(std::istream & is) {
		is.peek();
		return !is.good();
	}

	static std::string MatchPureString(std::istream & is);

	static void EraseSpace(std::istream & is);
	static Json MatchPrimary(std::istream & is);
	static Json MatchIdent(std::istream & is);
	static Json MatchNumber(std::istream & is);
	static Json MatchString(std::istream & is);
	static Json MatchArray(std::istream & is);
	static Json MatchObject(std::istream & is);
};


void JParser::EraseSpace(std::istream & is) {
	while(!isEof(is)) {
		char ch = is.peek();
		if(isspace(ch)) {
			is.ignore();
		} else {
			break;
		}
	}
}

Json JParser::MatchPrimary(std::istream & is) {
	EraseSpace(is);
	char ch = is.peek();

	switch(ch) {
		case '_':
		case 'a'...'z':
		case 'A'...'Z':
			return MatchIdent(is);
		case '+':
		case '-':
		case '0'...'9':
			return MatchNumber(is);
		case '\'':
		case '"':
			return MatchString(is);
		case '[':
			return MatchArray(is);
		case '{':
			return MatchObject(is);
		default:
			throw std::runtime_error("unexpected character meet whilc process primary token " + std::to_string(ch));
	}
}

Json JParser::MatchIdent(std::istream & is) {
	std::string token;
	while(!isEof(is)) {
		char ch = is.get();

		if(isalnum(ch) || ch == '_') {
			token.push_back(ch);
		} else {
			is.unget();
			break;
		}
	}

	if(token == "true") {
		return Json(true);
	} else if(token == "false") {
		return Json(false);
	} else if(token == "null") {
		return Json(nullptr);
	} else {
		throw std::runtime_error("unexpected token " + token);
	}
}

Json JParser::MatchNumber(std::istream & is) {
	enum { INT, FLOAT } type = INT;
	std::string token;
	while(!isEof(is)) {
		char ch = is.get();

		if(isdigit(ch) || ch == 'x' || ch == 'X'
				|| ch == '+' || ch == '-') {
			token.push_back(ch);
		} else if(ch == '.' || ch == 'e' || ch == 'E') {
			type = FLOAT;
			token.push_back(ch);
		} else {
			is.unget();
			break;
		}
	}

	int64_t integer;
	double floating;
	std::istringstream iss(token);
	if(type == INT) iss >> integer;
	else iss >> floating;

	if(!isEof(iss)) {
		throw std::runtime_error("invalid number " + token);
	}

	if(type == INT) return Json(integer);
	return Json(floating);
}


std::string JParser::MatchPureString(std::istream & is) {
	std::string token;
	EraseSpace(is);
	char quote = is.get();
	if(quote != '\"' && quote != '\'')
		throw std::runtime_error("expected a '\'' or '\"' while processing string\n");
	while(!isEof(is)) {
		char ch = is.get();

		if(ch == quote) {
			break;
		} else if(ch != '\\') {
			if(ch == '\n')
				throw std::runtime_error("line break in string literal");
			token.push_back(ch);
			continue;
		}

		is.get(ch);
		switch(ch) {
			case '\\': token.push_back('\\'); break;
			case 'n':  token.push_back('\n'); break;
			case 'r':  token.push_back('\r'); break;
			case 'f':  token.push_back('\f'); break;
			case 'v':  token.push_back('\v'); break;
			case 't':  token.push_back('\t'); break;
			case 'b':  token.push_back('\b'); break;
			case 'u':
			case 'U': {
				uint16_t u16 = 0;
				char *p = reinterpret_cast<char*>(&u16);
				is >> std::hex >> u16;
				if(u16 != static_cast<char>(u16))
				   token.push_back(p[1]);
				token.push_back(p[0]);

				if(is.fail()) {
					throw std::runtime_error("invalid unicode character");
				}
				break;
			}
			case 'x':
			case 'X':
				is >> std::hex >> ch;
				token.push_back(ch);

				if(is.fail()) {
					is.sync();
					throw std::runtime_error("invalid unicode character");
				}
				break;
			case '0':
				is >> std::oct >> ch;
				token.push_back(ch);
				break;
			case '\n':
				throw std::runtime_error("line break in string literal");
			default:
				token.push_back(ch);
				break;
		}
	}

	return token;
}

Json JParser::MatchString(std::istream &is) {
	return Json(MatchPureString(is));
}

Json JParser::MatchArray(std::istream & is) {
	Json::Array array;

	is.ignore();
	EraseSpace(is);
	char ch = is.peek();
	if(ch == ']') {
		is.ignore();
		return Json(std::move(array));
	}

	while(!isEof(is)) {
		array.push_back(MatchPrimary(is));

		EraseSpace(is);
		is.get(ch);

		if(ch == ']')
			break;
		else if(ch == ',')
			continue;
		else
			throw std::runtime_error("expected ',' or ']' while processing array, not " + ch);
	}

	return Json(std::move(array));
	// log("JT_ARRAY:%:%\n", to_string());
}

Json JParser::MatchObject(std::istream & is) {
	Json::Object object;
	is.ignore();
	EraseSpace(is);
	char ch = is.peek();
	if(ch == '}') {
		is.ignore();
		return Json(std::move(object));
	}

	while(!isEof(is)) {
		/* match key */
		std::string key = MatchPureString(is);

		/* match ':' */
		EraseSpace(is);
		is.get(ch);
		if(ch != ':') {
			throw std::runtime_error("missing colon while processing object");
		}

		/* match value */
		Json value = MatchPrimary(is);

		object.insert(std::make_pair<std::string, Json>(std::move(key), std::move(value)));

		EraseSpace(is);
		is.get(ch);

		if(ch == '}')
			break;
		else if(ch == ',')
			continue;
		else
			throw std::runtime_error("unexpected character '" + std::to_string(ch) + "'");
	}
	return Json(std::move(object));
}

Json Json::parse(const std::string &s) {
	try {
		std::istringstream iss(s);
		return JParser::MatchPrimary(iss);
	} catch(std::exception &e) {
		return Json();
	}
}

Json Json::parse(const std::string &s, std::string &err) {
	try {
		// IterStringStream iss(s);
		std::istringstream iss(s);
		return JParser::MatchPrimary(iss);
	} catch(std::exception &e) {
		err = e.what();
		return Json();
	}
}
