#pragma once

#if 0
#include"hash_processor.h"
#include"jenkins.h"
#include"crc.h"
#include"md5.h"
#include"sha.h"
namespace fast_io
{

using md5
#ifndef FAST_IO_NO_WARNING_DEPRECATED_CRYPTO_ALGOS
[[deprecated("The weaknesses of MD5 have been exploited in the field, most infamously by the Flame malware in 2012. See wikipedia https://en.wikipedia.org/wiki/MD5")]]
#endif
= sha<md5_function,false>;
}
#include"hmac.h"
#if defined(__GNUC__)
//#include"poly1305.h"
#endif

#endif
#include"md5_sha_hash_context.h"
