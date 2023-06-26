if constexpr(::std::endian::big==::std::endian::native)
{
	val=::fast_io::byte_swap(val);
}
for(;!(val&0x80u);++tofirst)
{
	*tofirst=val&0xFFu;
	++fromfirst;
	val>>=8u;
}
__builtin_memcpy(__builtin_addressof(val),fromfirst,sizeof(val));
