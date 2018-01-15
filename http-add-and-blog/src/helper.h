#include <array>
#include <istream>
#include <type_traits>
#include <cctype>
#include <utility>


inline void strip(std::string &s) {
	s.erase(0, s.find_first_not_of(" \t\r\n"));
	s.erase(s.find_last_not_of(" \t\r\n") + 1);
}


template<class Func, class... Args>
struct IsCallable {
	using yes = char [1];
	using no = char [2];

	template<class IFunc, class... IArgs>
	static yes& test(decltype(std::declval<IFunc>()(std::declval<IArgs>()...)) *);

	template<class IFunc, class... IArgs>
	static no& test(...);

	static constexpr int value = sizeof(test<Func, Args...>(0)) == sizeof(yes);
};

/*
template< 
    class CharT, 
	class Traits = std::char_traits<CharT>
> class basic_istream;
*/

template<class Container, class T>
bool contains(const Container &container, const T &val) {
	for(auto e : container)
		if(e == val)
			return true;
	return false;
}

template <class CharT, CharT... chars>
constexpr decltype(auto) operator "" _n() {
	return std::array<CharT, sizeof...(chars)> { chars... };
}


template<
	class CharT,
	class Traits = std::char_traits<CharT>,
	size_t N
>
auto ignore_until(
		std::basic_istream<CharT, Traits> &is,
		const std::array<CharT, N> &chars
	)
{
	while(is.good()) {
		if(contains(chars, is.peek()))
			return;
		is.ignore();
	}
}


template<
	class CharT,
	size_t N,
	class Traits = std::char_traits<CharT>
>
auto ignore_while(
		std::basic_istream<CharT, Traits> &is,
		const std::array<CharT, N> &chars
	)
{
	while(is.good()) {
		if(!contains(chars, is.peek()))
			return;
		is.ignore();
	}
}


template<
	class CharT,
	size_t N,
	class Traits = std::char_traits<CharT>,
	class Container
>
auto peek_while(
		std::basic_istream<CharT, Traits> &is,
		Container &container,
		const std::array<CharT, N> &chars
	)
{
	while(is.good()) {
		if(!contains(chars, is.peek()))
			return;
		container.push_back(is.get());
	}
}

template<
	class CharT,
	size_t N,
	class Traits = std::char_traits<CharT>,
	class Container
>
auto peek_until(
		std::basic_istream<CharT, Traits> &is,
		Container &container,
		const std::array<CharT, N> &chars
	)
{
	while(is.good()) {
		if(contains(chars, is.peek()))
			return;
		container.push_back(is.get());
	}
}


template<
	class CharT,
	class Traits = std::char_traits<CharT>,
	class Predicate
>
auto ignore_until(
		std::basic_istream<CharT, Traits> &is,
		Predicate &&predicate
	)
	-> std::enable_if_t<
		IsCallable<Predicate, CharT>::value,
		void
	>
{
	while(is.good() && !predicate(is.peek()))
		is.ignore();
}

template<
	class CharT,
	class Traits = std::char_traits<CharT>,
	class Predicate
>
auto ignore_while(
		std::basic_istream<CharT, Traits> &is,
		Predicate &&predicate
	)
	-> std::enable_if_t<
		IsCallable<Predicate, CharT>::value,
		void
	>
{
	while(is.good() && predicate(is.peek()))
		is.ignore();
}


template<
	class CharT,
	class Traits = std::char_traits<CharT>,
	class Container,
	class Predicate
>
auto peek_while(
		std::basic_istream<CharT, Traits> &is,
		Container &&container,
		Predicate &&predicate
	)
	-> std::enable_if_t<
		IsCallable<Predicate, CharT>::value,
		void
	>
{
	static_assert(std::is_same<typename std::remove_reference_t<Container>::value_type, CharT>::value, "");
	while(is.good() && predicate(is.peek()))
		container.push_back(is.get());
}


// ignore_space, ignore_blank, ignore_newline
// peek_word

template<class CharT, class Traits = std::char_traits<CharT>>
void ignore_space(std::basic_istream<CharT, Traits> &is) {
	ignore_while(is, [](char ch){return std::isspace(ch);});
}

template<class CharT, class Traits = std::char_traits<CharT>>
void ignore_blank(std::basic_istream<CharT, Traits> &is) {
	ignore_while(is, [](char ch){return std::isblank(ch);});
}

template<class CharT, class Traits = std::char_traits<CharT>>
void ignore_newline(std::basic_istream<CharT, Traits> &is) {
	ignore_while(is, "\r\n"_n);
}

template<class CharT, class Traits = std::char_traits<CharT>, class Container>
void peek_word(std::basic_istream<CharT, Traits> &is, Container &&container) {
	peek_while(is, container, [](char ch) {
			return ch == '_' || std::isalnum(ch);
	});
}


