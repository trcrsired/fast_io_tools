#include <fast_io.h>

template <bool line, typename T, typename... Args>
constexpr void print_controls_impl(T outsm, Args... args) {
  using chtype = typename T::output_char_type; // Extract the output character
                                               // type from the output stream

  // Determine if the output stream supports basic obuffer operations
  constexpr bool has_obuffer_ops{
      ::fast_io::operations::decay::defines::has_obuffer_basic_operations<T>};

  // Case 1: No arguments in the pack
  if constexpr (sizeof...(args) == 0) {
    if constexpr (line) {
      // Write a single newline character directly to the output stream
      ::fast_io::operations::decay::char_put_decay(
          outsm, ::fast_io::char_literal_v<u8'\n', chtype>);
    }
    return; // Exit early as there are no arguments to process
  }

  // Case 2: Arguments are provided in the pack
  else {
    // Scenario 1: Scatter-printable arguments
    if constexpr ((::fast_io::scatter_printable<chtype, Args> && ...)) {
      // If only one argument is provided and no newline is needed
      if constexpr (sizeof...(args) == 1 && !line) {
        // Directly write the single argument
        ::fast_io::operations::decay::write_all_decay(
            outsm, args...[0].base, args...[0].base + args...[0].len);
      }
      // If there are multiple arguments or a newline is required
      else {
        constexpr ::std::size_t n{
            sizeof...(Args) +
            static_cast<::std::size_t>(
                line)}; // Calculate the total number of scatters
        ::fast_io::basic_io_scatter_t<chtype>
            scatters[n]; // Allocate the scatter array

        // Populate the scatter array using compile-time indexing
        [&]<::std::size_t... I>(::std::index_sequence<I...>) {
          ((scatters[I] = args...[I]), ...);
        }(::std::make_index_sequence<sizeof...(Args)>{});

        // Add a line terminator scatter if "line" is true
        if constexpr (line) {
          scatters[n - 1] =
              ::fast_io::details::decay::line_scatter_common<chtype>;
        }

        // Write the entire scatter array to the output stream
        ::fast_io::operations::decay::scatter_write_all_decay(outsm, scatters,
                                                              n);
      }
    }

    // Scenario 2: Reserve-printable arguments
    else if constexpr ((::fast_io::reserve_printable<chtype, Args> && ...)) {
      // Calculate the total size of the buffer needed for all arguments
      constexpr ::std::size_t total_buffer_size{
          (print_reserve_size(::fast_io::io_reserve_type<chtype, Args>) + ...) +
          static_cast<::std::size_t>(
              line) // Include space for the newline if needed
      };
      chtype buffer[total_buffer_size]; // Allocate the buffer

      // Initialize the pointer to the start of the buffer
      chtype *current_ptr{buffer};
      bool inbuffer; // Indicates whether we can use the obuffer directly

      if constexpr (has_obuffer_ops) {
        // Retrieve the current obuffer pointers
        auto curr_ptr{obuffer_curr(outsm)};
        auto end_ptr{obuffer_end(outsm)};

        // Check if the total buffer size fits within the obuffer
        constexpr ::std::ptrdiff_t diff_buffer_size{
            static_cast<::std::ptrdiff_t>(total_buffer_size)};
        inbuffer = (diff_buffer_size < end_ptr - curr_ptr);
        if (inbuffer) [[likely]] {
          // Use the obuffer directly if space is sufficient
          current_ptr = curr_ptr;
        }
      }

      // Populate the buffer using print_reserve_define
      [&]<::std::size_t... I>(::std::index_sequence<I...>) {
        ((current_ptr =
              print_reserve_define(::fast_io::io_reserve_type<chtype, Args>,
                                   current_ptr, args...[I])),
         ...);
      }(::std::make_index_sequence<sizeof...(Args)>{});

      // Add a newline character to the buffer if "line" is true
      if constexpr (line) {
        *current_ptr = ::fast_io::char_literal_v<u8'\n', chtype>;
        ++current_ptr;
      }

      if constexpr (has_obuffer_ops) {
        if (inbuffer) [[likely]] {
          // Update the obuffer's current pointer and exit
          obuffer_set_curr(outsm, current_ptr);
          return;
        }
      }

      // Write the entire buffer to the output stream
      ::fast_io::operations::decay::write_all_decay(outsm, buffer, current_ptr);
    }
  }
}

int main() {
  constexpr ::fast_io::basic_io_scatter_t<char> bis{"Hello", 5};
  constexpr ::fast_io::basic_io_scatter_t<char> bis2{"Hello6", 6};
  constexpr ::fast_io::basic_io_scatter_t<char> bis3{"Hello8f", 7};
  print_controls_impl<false>(::fast_io::c_stdout(), bis, bis2, bis3, ::fast_io::io_print_alias(30));
}
