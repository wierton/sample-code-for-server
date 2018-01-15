#ifndef JSON_H
#define JSON_H


#include <map>
#include <string>
#include <memory>
#include <vector>
#include <cstdint>
#include <initializer_list>


class JsonElement;

class Json {
	std::shared_ptr<JsonElement> je;
public:
	using Array = std::vector<Json>;
	using Object = std::map<std::string, Json>;

	static Json null;

	enum Type {
		NONE, NIL, BOOLEAN, NUMBER, STRING, ARRAY, OBJECT,
	};

	Json();
	Json(nullptr_t);
	Json(bool);
	Json(int i);
	Json(int64_t i);
	Json(double f);
	Json(const char *s);
	Json(const std::string &s);
	Json(std::string &&s);
	Json(const Array &obj);
	Json(Array &&obj);
	Json(const Object &obj);
	Json(Object &&obj);

	Json(std::initializer_list<Json> &&);

	Json(const Json &) = default;
	Json(Json &&);
	Json &operator= (const Json &) = default;
	Json &operator= (Json &&);

	Json clone(); // deep copy

	static Json parse(const std::string &s);
	static Json parse(const std::string &s, std::string &err);

	bool is_null() const;
	bool is_boolean() const;
	bool is_number() const;
	bool is_integer() const;
	bool is_float() const;
	bool is_string() const;
	bool is_array() const;
	bool is_object() const;

	bool to_boolean() const;
	int64_t to_integer() const;
	double to_float() const;
	std::string to_string() const;

	const std::string &as_string() const;
	const Array &as_array() const;
	const Object &as_object() const;

	size_t size() const;

	Json &operator[](const size_t idx);
	Json &operator[](const std::string &s);

	friend bool operator== (const Json &, const Json &);
	friend bool operator!= (const Json &, const Json &);
	friend Json operator+ (const Json &, const Json &);
	friend Json operator- (const Json &, const Json &);
	friend Json operator* (const Json &, const Json &);
	friend Json operator/ (const Json &, const Json &);
	friend Json operator% (const Json &, const Json &);

	friend std::ostream &operator <<(std::ostream &, const Json &);

	Json &operator+= (const Json &);
	Json &operator-= (const Json &);
};


#endif
