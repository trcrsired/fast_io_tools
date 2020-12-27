#pragma once

namespace fast_io
{

/*
https://docs.oracle.com/javase/7/docs/api/java/math/RoundingMode.html

CEILING
Rounding mode to round towards positive infinity.
DOWN
Rounding mode to round towards zero.
FLOOR
Rounding mode to round towards negative infinity.
HALF_DOWN
Rounding mode to round towards "nearest neighbor" unless both neighbors are equidistant, in which case round down.
HALF_EVEN
Rounding mode to round towards the "nearest neighbor" unless both neighbors are equidistant, in which case, round towards the even neighbor.
HALF_UP
Rounding mode to round towards "nearest neighbor" unless both neighbors are equidistant, in which case round up.
UNNECESSARY
Rounding mode to assert that the requested operation has an exact result, hence no rounding is necessary.
UP
Rounding mode to round away from zero.

*/

enum class rounding_mode
{
unnecessary,
ceiling,
down,
floor,
half_down,
half_even,
half_up,
up,
};

}