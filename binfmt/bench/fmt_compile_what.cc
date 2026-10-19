// What is FMT_COMPILE actually? Print the compiler-visible type of the
// compiled format object for a few format strings.
//
// If FMT_COMPILE produced a byte program like binfmt, we would see an array
// or a buffer type. Instead we see nested structs: concat<text<...>,
// concat<field<...>, ...>>.

#include <cstddef>
#include <cstdio>
#include <typeinfo>

#include <fmt/format.h>
#include <fmt/compile.h>

template <typename T> void show(char const *label)
{
	::std::printf("%-42s : %s\n", label, typeid(T).name());
}

int main()
{
	using namespace fmt::detail;

	// "hello {} world {} hello\n" with two size_t
	show<decltype(compile<::std::size_t, ::std::size_t>(
		FMT_COMPILE("hello {} world {} hello\n")))>("hello {} world {} hello");
	show<decltype(compile<::std::size_t>(FMT_COMPILE("{}")))>("{}");
	show<decltype(compile<::std::size_t, ::std::size_t, ::std::size_t>(
		FMT_COMPILE("hello {2} world {1} fmtslow\n")))>("hello {2} world {1}");

	::std::printf("\nsizeof compiled objects:\n");
	::std::printf("  %%-program  : %zu bytes\n",
				  sizeof(decltype(compile<::std::size_t>(FMT_COMPILE("{}")))));
	::std::printf("  multi       : %zu bytes\n",
				  sizeof(decltype(compile<::std::size_t, ::std::size_t>(
					  FMT_COMPILE("hello {} world {} hello\n")))));
	return 0;
}
