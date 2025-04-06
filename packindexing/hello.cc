#if 0
#include<fast_io.h>

void test(size_t v);

template<std::size_t n,typename Arg>
inline void test_rg(Arg arg)
{
	::fast_io::io::debug_println(n,"\t",arg);
}

template<typename T, typename... Args>
inline void print_controls_impl(T outsm, Args ...args)
{
	auto const [...I] = ::std::make_index_sequence<sizeof...(args)>{};
	(test_rg<I>(args...[I]),...);
}

int main()
{
	print_controls_impl(3,4,5,10);
}

#else

#include <iostream>
#include <utility>

// Variadic template function
template <typename... Args>
void print_indices_and_values(Args... args) {
    // Create compile-time index sequence
    [&]<std::size_t... I>(std::index_sequence<I...>) {
        // Use fold expression to print indices and values
        ((std::cout << "Index: " << I+3 << ", Value: " << args...[I+3] << "\n"), ...);
    }(std::make_index_sequence<sizeof...(args)-3>{}); // Pass the index sequence
}

int main() {
    print_indices_and_values(10, 20, 30,40,50,60,70,80);
    // Expected Output:
    // Index: 0, Value: 10
    // Index: 1, Value: 20
    // Index: 2, Value: 30
    return 0;
}



#endif