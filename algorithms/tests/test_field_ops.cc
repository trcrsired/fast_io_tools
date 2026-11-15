#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

namespace
{

namespace ops = ::fast_io::algorithms::field_ops;

// The objects are not class templates: the operand types are deduced.
static_assert(ops::add{}.compute(2, 3) == 5);
static_assert(ops::add{}.compute(2, 3LL) == 5);
static_assert(ops::minus{}.compute(7, 3) == 4);
static_assert(ops::multiply{}.compute(4, 5) == 20);
static_assert(ops::divide{}.compute(9, 3) == 3);
static_assert(ops::modulo{}.compute(10, 4) == 2);
static_assert(ops::negate{}.compute(7) == -7);
static_assert(ops::bit_and{}.compute(6, 3) == 2);
static_assert(ops::bit_or{}.compute(6, 3) == 7);
static_assert(ops::bit_xor{}.compute(6, 3) == 5);
static_assert(ops::bit_not{}.compute(0) == ~0);
static_assert(ops::logical_and{}.compute(1, 0) == false);
static_assert(ops::logical_or{}.compute(1, 0) == true);
static_assert(ops::logical_not{}.compute(0) == true);
static_assert(ops::minimum{}.compute(3, 5) == 3);
static_assert(ops::maximum{}.compute(3, 5) == 5);

static_assert(noexcept(ops::add{}.compute(1, 2)));
static_assert(noexcept(ops::multiply{}.compute(1, 2)));
static_assert(noexcept(ops::negate{}.compute(1)));
static_assert(noexcept(ops::bit_xor{}.compute(1, 2)));
static_assert(noexcept(ops::logical_and{}.compute(1, 2)));
static_assert(noexcept(ops::minimum{}.compute(1, 2)));
static_assert(noexcept(ops::maximum{}.compute(1, 2)));

} // namespace

int main()
{
  if (ops::add{}.compute(1.5, 2.5) != 4.0)
  {
    return 1;
  }
  if (ops::divide{}.compute(7.0, 2.0) != 3.5)
  {
    return 2;
  }
  if (ops::minimum{}.compute(2.5, 1.5) != 1.5)
  {
    return 3;
  }
  return 0;
}
