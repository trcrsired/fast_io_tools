
#if __has_builtin(__builtin_shuffle) && __has_builtin(__builtin_bit_cast)
	void to_big_endian() noexcept
		requires(simd_shuffle_size_is_supported(sizeof(vec_type))&&(sizeof(N)!=1)&&std::integral<value_type>&&(std::endian::little==std::endian::native||std::endian::big==std::endian::native))
	{
		if constexpr(std::endian::little==std::endian::native)
		{
			using value_type2 [[gnu::__vector_size__ (sizeof(vec_type))]] = char;
			auto val_shuffle{__builtin_bit_cast(value_type2,value)};
			if constexpr(sizeof(value_type)==8)
			{
				if constexpr(sizeof(Z)==16)
				{
					value=__builtin_bit_cast(vec_type,__builtin_shuffle(val_shuffle,val_shuffle,7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8));
				}
				else if constexpr(sizeof(char_type)==32)
				{
					value=__builtin_bit_cast(vec_type,__builtin_shuffle(val_shuffle,val_shuffle,7,6,5,4,3,2,1,0,15,14,13,12,11,10,9,8,23,22,21,20,19,18,17,16,31,30,29,28,27,26,25,24));
				}
			}
			else if constexpr(sizeof(value_type)==4)
			{
				if constexpr(sizeof(Z)==16)
				{
					value=__builtin_bit_cast(vec_type,__builtin_shuffle(val_shuffle,val_shuffle,3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12));
				}
				else if constexpr(sizeof(char_type)==32)
				{
					value=__builtin_bit_cast(vec_type,__builtin_shuffle(val_shuffle,val_shuffle,3,2,1,0,7,6,5,4,11,10,9,8,15,14,13,12,19,18,17,16,23,22,21,20,27,26,25,24,31,30,29,28));
				}
			}
		}
	}
#endif