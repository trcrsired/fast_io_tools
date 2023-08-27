#pragma once

namespace fast_io
{

namespace details
{

inline constexpr bool is_utf16_surrogate(char16_t uc) noexcept { return (uc - 0xd800u) < 2048u; }
inline constexpr bool is_utf16_high_surrogate(char16_t uc) noexcept { return (uc & 0xfffffc00) == 0xd800; }
inline constexpr bool is_utf16_low_surrogate(char16_t uc) noexcept { return (uc & 0xfffffc00) == 0xdc00; }

inline constexpr char32_t utf16_surrogate_to_utf32(char16_t high, char16_t low) noexcept
{ 
	return static_cast<char32_t>((static_cast<std::uint_least32_t>(high) << 10u) + low - 0x35fdc00u); 
}

template<typename T>
inline constexpr deco_result<char8_t,typename T::output_char_type> utf16_generic_impl(
	char16_t const *fromfirst,char16_t const *fromlast,
	typename T::output_char_type *tofirst,typename T::output_char_type *tolast) noexcept
{
	using output_char_type = typename T::output_char_type;
	constexpr
		::std::size_t N{::fast_io::intrinsics::optimal_simd_vector_run_with_cpu_instruction_size};
	using temp_simd_vector_type = ::fast_io::intrinsics::simd_vector<::std::uint_least16_t,(N/sizeof(char16_t))>;

#if (__cpp_lib_bit_cast >= 201806L) && !defined(__clang__)
	constexpr
#endif
		auto cmp128{::fast_io::details::utfconstantsimd_impl<0x80u,char16_t,N/sizeof(char16_t)>};

	using simd_vector_type = ::fast_io::intrinsics::simd_vector<::std::uint_least8_t,N>;
	simd_vector_type svec0,svec1;

	while(fromfirst!=fromlast&&tofirst!=tolast)
	{
		char16_t v0{*fromfirst};
		if constexpr(!::std::same_as<char_type,char32_t>)
		{
			if(v0<0x80u)
			{
				*tofirst=static_cast<char_type>(v0);
				++tofirst;
				++fromfirst;

				if constexpr(N==0)
				{
					::std::size_t fromdiff2{static_cast<::std::size_t>(fromlast-fromfirst)};
					::std::size_t todiff2{static_cast<::std::size_t>(tolast-tofirst)};
					if(todiff2<fromdiff2)
					{
						fromdiff2=todiff2;
					}
					constexpr
						::std::size_t ndiffmask{static_cast<::std::size_t>(N-1u)};
					constexpr
						int ndiffshift{::std::bit_width(ndiffmask)};
					::std::size_t ndiff{(fromdiff2>>ndiffshift)};
					for(;ndiff;--ndiff)
					{
						if constexpr(N==16)
						{
							svec0.load(fromfirst);
							svec1.load(fromfirst+8);
						}
						else if constexpr(N==32)
						{
							svec0.load(fromfirst);
							svec1.load(fromfirst+16);
						}
						else
						{
							svec0.load(fromfirst);
							svec1.load(fromfirst+32);
						}
						if(!is_all_zeros((static_cast<temp_simd_vector_type>(svec0)<cmp128)|
							(static_cast<temp_simd_vector_type>(svec1)<cmp128)))
						{
							break;
						}
						if constexpr(N==16)
						{
							svec0.value=__builtin_shufflevector(svec0.value,svec1.value,
							0,2,4,6,8,10,12,14,-1,-1,-1,-1,-1,-1,-1,-1);
							svec2.value=__builtin_shufflevector(svec2.value,svec3.value,
							-1,-1,-1,-1,-1,-1,-1,-1,0,2,4,6,8,10,12,14);
							svec0.value=__builtin_shufflevector(svec0.value,svec2.value,
							0,1,2,3,4,5,6,7,24,25,26,27,28,29,30,31);
						}
						else if constexpr(N==32)
						{
							svec0.value=__builtin_shufflevector(svec0.value,svec1.value,
							0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
							-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1);
							svec2.value=__builtin_shufflevector(svec2.value,svec3.value,
							-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60);
							svec0.value=__builtin_shufflevector(svec0.value,svec2.value,
							0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
							48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63);
						}
						else if constexpr(N==64)
						{
							svec0.value=__builtin_shufflevector(svec0.value,svec1.value,
							0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
							64,68,72,76,80,84,88,92,96,100,104,108,112,116,120,
							-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1);
							svec2.value=__builtin_shufflevector(svec2.value,svec3.value,
							-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
							0,4,8,12,16,20,24,28,32,36,40,44,48,52,56,60,
							64,68,72,76,80,84,88,92,96,100,104,108,112,116,120);
							svec0.value=__builtin_shufflevector(svec0.value,svec2.value,
							0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
							16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
							96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,
							112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127);
						}
						svec0.store(tofirst);
						fromfirst+=N;
						tofirst+=N;
					}
				}
				if(!ndiff)
				{
					break;
				}
				::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};
				::std::size_t todiff{static_cast<::std::size_t>(tolast-tofirst)};
				if(todiff<fromdiff)
				{
					fromdiff=todiff;
				}
				for(;fromdiff;--fromdiff)
				{
					v0=*fromfirst;
					if(0x80u<=v0)
					{
						break;
					}
					*tofirst=static_cast<char_type>(v0);
					++tofirst;
					++fromfirst;
				}
				if(!fromdiff)
				{
					break;
				}
			}

		}
		if(!is_utf16_surrogate(v0))
		{
			if constexpr(::std::same_as<char_type,char32_t>)
			{
				*tofirst=static_cast<char_type>(v0);
				++tofirst;
				++fromfirst;
				::std::size_t fromdiff{static_cast<::std::size_t>(fromlast-fromfirst)};
				::std::size_t todiff{static_cast<::std::size_t>(tolast-tofirst)};
				if(todiff<fromdiff)
				{
					fromdiff=todiff;
				}
				for(;fromdiff;--fromdiff)
				{
					v0=*fromfirst;
					if(is_utf16_surrogate(v0))
					{
						break;
					}
					*tofirst=static_cast<char_type>(v0);
					++tofirst;
					++fromfirst;
				}
				if(!fromdiff)
				{
					break;
				}
			}
			else
			{
				auto ret{T::get_code_point(tofirst,static_cast<std::size_t>(tolast-tofirst),v0)};
				if(!ret)
				{
					break;
				}
				tofirst+=ret;
				++fromfirst;
				continue;
			}
		}
		char32_t codepoint{0xFFFD};
		::std::size_t tomov{1};
		if (is_utf16_high_surrogate(v0))
		{
			if (fromfirst+1 == fromlast)
			{
				break;
			}
			auto v1{fromfirst[1]};
			if (is_utf16_low_surrogate(v1))
			{
				codepoint = utf16_surrogate_to_utf32(v0,v1);
				++tomov;
			}
		}
		if constexpr(::std::same_as<char_type,char32_t>)
		{
			*tofirst=codepoint;
			++tofirst;
			fromfirst+=tomov;
		}
		else
		{
			auto ret{T::get_code_point(tofirst,static_cast<std::size_t>(tolast-tofirst),codepoint)};
			if(!ret)
			{
				break;
			}
			tofirst+=ret;
			fromfirst+=tomov;
		}
	}
	return {fromfirst,tofirst};
}

}

}
