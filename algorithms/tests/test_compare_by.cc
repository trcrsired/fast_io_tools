#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

#include <cstddef>
#include <utility>

namespace
{

struct record
{
  int key;
  int payload;
};

struct by_payload
{
  inline constexpr int const &operator()(record const &r) const noexcept
  {
    return r.payload;
  }
};

struct descending
{
  template<typename T>
  inline constexpr auto compare(T const &a, T const &b) const noexcept
  {
    return b <=> a;
  }
};

constexpr bool constexpr_compare_by_test()
{
  record a[]{record{3, 1}, record{1, 2}, record{2, 3}};
  ::fast_io::algorithms::sort(a, ::fast_io::algorithms::compare_by{&record::key});
  if (!(a[0].key == 1 && a[1].key == 2 && a[2].key == 3))
  {
    return false;
  }
  ::fast_io::algorithms::sort(
    a, ::fast_io::algorithms::compare_by{&record::key, descending{}});
  if (!(a[0].key == 3 && a[1].key == 2 && a[2].key == 1))
  {
    return false;
  }
  ::fast_io::algorithms::sort(
    a, ::fast_io::algorithms::descend_compare_by{&record::key});
  return a[0].key == 3 && a[1].key == 2 && a[2].key == 1;
}

static_assert(constexpr_compare_by_test());

static_assert(noexcept(
  ::fast_io::algorithms::compare_by{&record::key}.compare(
    ::std::declval<record const &>(), ::std::declval<record const &>())));

static_assert(noexcept(
  ::fast_io::algorithms::compare_by{&record::key}.equal(
    ::std::declval<record const &>(), ::std::declval<record const &>())));

static_assert(noexcept(
  ::fast_io::algorithms::descend_compare_by{&record::key}.compare(
    ::std::declval<record const &>(), ::std::declval<record const &>())));

} // namespace

int main()
{
  record a[]{record{3, 1}, record{1, 3}, record{2, 2}};

  // Member pointer projection.
  ::fast_io::algorithms::sort(a, ::fast_io::algorithms::compare_by{&record::key});
  if (!(a[0].key == 1 && a[1].key == 2 && a[2].key == 3))
  {
    return 1;
  }

  // Projection plus a custom comparator over the projected key.
  ::fast_io::algorithms::sort(
    a, ::fast_io::algorithms::compare_by{&record::key, descending{}});
  if (!(a[0].key == 3 && a[1].key == 2 && a[2].key == 1))
  {
    return 2;
  }

  // Descending projection.
  ::fast_io::algorithms::sort(
    a, ::fast_io::algorithms::descend_compare_by{&record::key});
  if (!(a[0].key == 3 && a[1].key == 2 && a[2].key == 1))
  {
    return 6;
  }

  // Descending projection with an inner descending comparator is ascending.
  ::fast_io::algorithms::sort(
    a, ::fast_io::algorithms::descend_compare_by{&record::key, descending{}});
  if (!(a[0].key == 1 && a[1].key == 2 && a[2].key == 3))
  {
    return 7;
  }

  // Named projection type.
  ::fast_io::algorithms::sort(
    a, ::fast_io::algorithms::compare_by{by_payload{}});
  if (!(a[0].payload == 1 && a[1].payload == 2 && a[2].payload == 3))
  {
    return 3;
  }

  // Equality goes through the projected key.
  record b[]{record{1, 7}, record{1, 7}, record{2, 8}};
  auto const end{::fast_io::algorithms::unique(
    b, ::fast_io::algorithms::compare_by{&record::key})};
  if (end - b != 2)
  {
    return 4;
  }

  // Binary search compares records by the projected key.
  ::fast_io::algorithms::sort(a, ::fast_io::algorithms::compare_by{&record::key});
  record const key{2, 0};
  auto const found{::fast_io::algorithms::lower_bound(
    a, key, ::fast_io::algorithms::compare_by{&record::key})};
  if (found == a + 3 || found->key != 2)
  {
    return 5;
  }
  return 0;
}
