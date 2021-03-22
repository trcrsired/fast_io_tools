#pragma once

namespace fast_io::win32
{
[[gnu::dllimport]] extern void* __stdcall RtlSecureZeroMemory(void* __restrict,std::size_t) noexcept;
}