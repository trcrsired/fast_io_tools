#pragma once

#include"sha256.h"
#include"sha_impl.h"
#include"sha1.h"
#include"sha512.h"

namespace fast_io
{

using sha1
#ifndef FAST_IO_NO_WARNING_DEPRECATED_CRYPTO_ALGOS
[[deprecated("SHA1 is no longer a secure algorithm. See wikipedia https://en.wikipedia.org/wiki/SHA-1")]]
#endif
= sha<sha1_function>;

using sha512 = sha<sha512_function,true,16>;

}