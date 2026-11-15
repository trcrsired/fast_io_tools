#pragma once
// ADL customization points for the algorithms in this directory.
//
// A container or iterator author can provide an overload of
// `algorithms_define` in a namespace associated with the iterator or the tag:
//
//   template<typename... Args>
//   auto algorithms_define(
//     ::fast_io::algorithms::customization::define_type<
//       ::fast_io::algorithms::customization::copy_tag, Args...>,
//     ...) noexcept -> ...;
//
// The overload must be `noexcept`. A throwing overload is ignored and the
// generic implementation is used. The tag types below are the stable spelling
// users should overload on. The algorithm arguments are passed separately.
namespace fast_io::algorithms::customization
{

template<typename Tag>
struct define_type
{
  explicit constexpr define_type() noexcept = default;
};

template<typename Tag>
inline constexpr define_type<Tag> define{};

// Makes the customization name visible to the parser without providing a
// viable fallback. User overloads are found by ADL on the tag (whose namespace
// is this one) and on the algorithm arguments.
struct customization_probe;
template<typename... Args>
void algorithms_define(customization_probe, Args&&...) = delete;

// Matches only noexcept overloads so the generic algorithms keep their
// existing noexcept contract. Associated namespaces of the tag and of every
// argument are searched by ADL.
template<typename Tag, typename... Args>
concept has_define = requires(Args&&... args)
{
  { algorithms_define(define_type<Tag>(), ::std::forward<Args>(args)...) } noexcept;
};

template<typename Tag, typename... Args>
inline constexpr bool has_define_args(Args&&...)
{
  return has_define<Tag, Args...>;
}

template<typename Tag, typename... Args>
inline constexpr decltype(auto) dispatch_define(Args&&... args) noexcept
{
  return algorithms_define(define_type<Tag>(), ::std::forward<Args>(args)...);
}
struct adjacent_find_tag {};
struct advance_tag {};
struct all_of_tag {};
struct any_of_tag {};
struct binary_search_tag {};
struct clamp_tag {};
struct compare_tag {};
struct copy_tag {};
struct copy_backward_tag {};
struct copy_if_tag {};
struct copy_n_tag {};
struct count_tag {};
struct count_if_tag {};
struct destroy_tag {};
struct destroy_at_tag {};
struct destroy_n_tag {};
struct equal_tag {};
struct equal_range_tag {};
struct fill_tag {};
struct fill_n_tag {};
struct find_tag {};
struct find_end_tag {};
struct find_first_of_tag {};
struct find_if_tag {};
struct find_if_not_tag {};
struct for_each_tag {};
struct for_each_n_tag {};
struct generate_tag {};
struct generate_n_tag {};
struct includes_tag {};
struct set_union_tag {};
struct set_symmetric_difference_tag {};
struct set_intersection_tag {};
struct set_difference_tag {};
struct inplace_merge_tag {};
struct is_heap_tag {};
struct is_heap_until_tag {};
struct is_partitioned_tag {};
struct is_permutation_tag {};
struct is_permutation_prefix_tag {};
struct is_sorted_tag {};
struct is_sorted_until_tag {};
struct iter_swap_tag {};
struct lexicographical_compare_tag {};
struct lexicographical_compare_three_way_tag {};
struct lower_bound_tag {};
struct make_heap_tag {};
struct max_tag {};
struct max_element_tag {};
struct merge_tag {};
struct min_tag {};
struct min_element_tag {};
struct minmax_tag {};
struct minmax_element_tag {};
struct mismatch_tag {};
struct move_tag {};
struct move_backward_tag {};
struct next_permutation_tag {};
struct non_overlapped_copy_tag {};
struct non_overlapped_copy_n_tag {};
struct none_of_tag {};
struct nth_element_tag {};
struct partial_sort_tag {};
struct partial_sort_copy_tag {};
struct partition_tag {};
struct partition_copy_tag {};
struct partition_point_tag {};
struct pop_heap_tag {};
struct prev_permutation_tag {};
struct push_heap_tag {};
struct remove_tag {};
struct remove_if_tag {};
struct replace_tag {};
struct replace_copy_tag {};
struct replace_copy_if_tag {};
struct replace_if_tag {};
struct reverse_tag {};
struct reverse_copy_tag {};
struct rotate_tag {};
struct rotate_copy_tag {};
struct search_tag {};
struct search_n_tag {};
struct shift_left_tag {};
struct shift_right_tag {};
struct sort_tag {};
struct sort_heap_tag {};
struct stable_partition_tag {};
struct swap_tag {};
struct swap_ranges_tag {};
struct transform_tag {};
struct transform_binary_tag {};
struct uninitialized_copy_tag {};
struct uninitialized_copy_backward_tag {};
struct uninitialized_copy_n_tag {};
struct uninitialized_default_construct_tag {};
struct uninitialized_default_construct_n_tag {};
struct uninitialized_fill_tag {};
struct uninitialized_fill_n_tag {};
struct uninitialized_move_tag {};
struct uninitialized_move_backward_tag {};
struct uninitialized_move_n_tag {};
struct uninitialized_relocate_tag {};
struct uninitialized_relocate_backward_tag {};
struct uninitialized_relocate_n_tag {};
struct uninitialized_value_construct_tag {};
struct uninitialized_value_construct_n_tag {};
struct unique_tag {};
struct unique_copy_tag {};
struct upper_bound_tag {};

} // namespace fast_io::algorithms::customization

// Dispatch to an ADL customization when one exists. The customization must
// return the same type as the enclosing algorithm and must be noexcept.
#define FAST_IO_ALGORITHMS_CUSTOMIZE(tag, ...)                                 \
  if constexpr (::fast_io::algorithms::customization::has_define_args<tag>(   \
                  __VA_ARGS__))                                               \
  {                                                                            \
    return ::fast_io::algorithms::customization::dispatch_define<tag>(         \
      __VA_ARGS__);                                                            \
  }
