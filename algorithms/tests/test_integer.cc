#include <fast_io.h>
#include <fast_io_dsal/impl/misc/push_macros.h>
#include "../impl.h"
#include <fast_io_dsal/impl/misc/pop_macros.h>

namespace
{

static_assert(::fast_io::algorithms::gcd(12, 18) == 6);
static_assert(::fast_io::algorithms::gcd(18, 12) == 6);
static_assert(::fast_io::algorithms::gcd(-12, 18) == 6);
static_assert(::fast_io::algorithms::gcd(12, -18) == 6);
static_assert(::fast_io::algorithms::gcd(-12, -18) == 6);
static_assert(::fast_io::algorithms::gcd(0, 0) == 0);
static_assert(::fast_io::algorithms::gcd(0, 7) == 7);
static_assert(::fast_io::algorithms::gcd(7, 0) == 7);
static_assert(::fast_io::algorithms::gcd(18u, 12u) == 6u);

static_assert(::fast_io::algorithms::lcm(4, 6) == 12);
static_assert(::fast_io::algorithms::lcm(-4, 6) == 12);
static_assert(::fast_io::algorithms::lcm(4, -6) == 12);
static_assert(::fast_io::algorithms::lcm(-4, -6) == 12);
static_assert(::fast_io::algorithms::lcm(0, 5) == 0);
static_assert(::fast_io::algorithms::lcm(5, 0) == 0);
static_assert(::fast_io::algorithms::lcm(4u, 6u) == 12u);

static_assert(noexcept(::fast_io::algorithms::gcd(1, 2)));
static_assert(noexcept(::fast_io::algorithms::lcm(1, 2)));

} // namespace

int main()
{
	if (::fast_io::algorithms::gcd(1071, 462) != 21)
	{
		return 1;
	}
	if (::fast_io::algorithms::lcm(21, 6) != 42)
	{
		return 2;
	}
	if (::fast_io::algorithms::gcd(static_cast<unsigned long long>(1) << 40,
			static_cast<unsigned long long>(1) << 20) !=
		(static_cast<unsigned long long>(1) << 20))
	{
		return 3;
	}
	return 0;
}
