#include<fast_io.h>

template<bool line, typename T, typename... Args>
constexpr void print_controls_impl(T outsm, Args... args) {
    using chtype = typename T::char_type; // Extract the character type from the output stream

    // Case 1: No arguments in the pack
    if constexpr (sizeof...(args) == 0) {
        if constexpr (line) {
            // Write a single newline character using char_put_decay for efficiency
            ::fast_io::operations::decay::char_put_decay(outsm, ::fast_io::char_literal_v<u8'\n', chtype>);
        }
        return; // Exit as there are no arguments to process
    }

    // Case 2: There are arguments in the pack
    else {
        // Ensure all arguments are scatter-printable for the specified character type
        if constexpr ((::fast_io::scatter_printable<chtype, Args> && ...)) {
            // Special case: Single argument and "line" is false
            if constexpr (sizeof...(args) == 1 && !line) {
                // Directly write the single argument to the output stream
                ::fast_io::operations::decay::write_all_decay(outsm, args...[0].base, args...[0].base + args...[0].len);
            } 
            // General case: Multiple arguments or "line" is true
            else {
                constexpr ::std::size_t n{sizeof...(Args) + static_cast<::std::size_t>(line)}; // Calculate scatter size
                ::fast_io::basic_io_scatter_t<chtype> scatters[n]; // Initialize scatter array

                // Populate the scatter array using an index-based approach
                [&]<::std::size_t... I>(::std::index_sequence<I...>) {
                    (
                        (scatters[I] = (args...[I])), ...
                    );
                }(::std::make_index_sequence<sizeof...(Args)>{}); // Generate indices for argument unpacking

                // Add a line terminator scatter if "line" is true
                if constexpr (line) {
                    scatters[n - 1] = ::fast_io::details::decay::line_scatter_common<chtype>;
                }

                // Write all scatter elements to the output stream
                ::fast_io::operations::decay::scatter_write_all_decay(
                    outsm, scatters, scatters + n
                );
            }
        }
    }
}


int main()
{
    ::fast_io::c_io_observer_unlocked ciobulk{stdout};
	print_controls_impl<true>(ciobulk, "Hello");
}
