namespace std { inline namespace _LIBCPP_ABI_NAMESPACE {
template <class _CharT>
class __stdinbuf{};
template <class _CharT>
class __stdoutbuf{};
}}

void foo(std::__stdinbuf<char32_t>){};

/*
std::__stdinbuf<_CharT>
Itanium:
char: NSt21_LIBCPP_ABI_NAMESPACE10__stdinbufIcEE
wchar_t: NSt21_LIBCPP_ABI_NAMESPACE10__stdinbufIwEE
char8_t: NSt21_LIBCPP_ABI_NAMESPACE10__stdinbufIDuEE
char16_t: NSt21_LIBCPP_ABI_NAMESPACE10__stdinbufIDsEE
char32_t: NSt21_LIBCPP_ABI_NAMESPACE10__stdinbufIDiEE
MSVC:

*/

#ifdef PRINTOUT
#include<typeinfo>
#include<stdio.h>

int main()
{
	printf("char: %s\n"
		"char: %s\n"
		"char: %s\n"
		"char: %s\n"
		"char: %s\n",typeid(std::__stdinbuf<char>).name(),
		typeid(std::__stdinbuf<wchar_t>).name(),
		typeid(std::__stdinbuf<char8_t>).name(),
		typeid(std::__stdinbuf<char16_t>).name(),
		typeid(std::__stdinbuf<char32_t>).name());
}
#endif