#pragma once

namespace fast_io
{

enum class posix_family
{
posix_libc_call,
os_system_call,
native=os_system_call,
#if defined(__linux__) || defined(__wasi__) || defined(__MSDOS__)
os_system_call
#else
posix_libc_call
#endif
};

}
