#pragma once
#include"pe_header.h"

namespace pe_file_format
{

template<typename T>
inline constexpr T const& dereference(char const* ptr,char const* last) noexcept
{
	if(static_cast<std::size_t>(last-ptr)<sizeof(T))
		panic();
	return *reinterpret_cast<T const*>(ptr);
}

inline  constexpr char const* check_ptr_advance(char const* ptr,char const* last,std::size_t pos) noexcept
{
	if(ptr+pos>last)
		panic();
	return ptr+pos;
}

struct pe_file
{
	image_dos_header const& dosheader{};
	pe_header const& peheader{};
	pe32_optional_header const& peoptionalheader{};
};

inline constexpr pe_file deserialize(char const* first,char const* last) noexcept
{
	auto& dosheader{dereference<image_dos_header>(first,last)};
	if(dosheader.e_magic!=0x5A4D)
		panicln("dos header magic is not 0x5a4d (MZ) it is 0x",fast_io::mnp::hex(dosheader.e_magic));
	auto& peheader{dereference<pe_header>(check_ptr_advance(first,last,dosheader.e_lfanew),last)};
	if(peheader.mMagic!=0x4550)
		panicln("pe header magic is not 0x00004550 (PE00) it is 0x",fast_io::mnp::hex(peheader.mMagic));

	auto& peoptionalheader{dereference<pe32_optional_header>(check_ptr_advance(first,last,dosheader.e_lfanew+sizeof(peheader)),last)};

	return {dosheader,peheader,peoptionalheader};
}

}
