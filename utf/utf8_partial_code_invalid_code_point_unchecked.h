if constexpr(::std::same_as<output_char_type,char32_t>)
{
	*tofirst=0xFFFD;
	++tofirst;
}
else
{
	T::get_invalid_code_points(tofirst);
	tofirst+=invalidcodepointslen;
}
++fromfirst;
