::std::size_t ndiff2{(fromdiff2>>2)+((fromdiff2&0x3u)!=0u)};
do
{
	__builtin_memcpy(__builtin_addressof(val),fromfirst,sizeof(val));
	if((val&0x80808080u)!=0u)
	{
		break;
	}
	low=val&0xFFFFu;
	high=val>>16u;
	if constexpr(::std::endian::big==::std::endian::native)
	{
		*tofirst=(high&0xFF);
		tofirst[1]=(high>>8u);
		tofirst[2]=(low&0xFF);
		tofirst[3]=(low>>8u);
	}
	else
	{
		*tofirst=(low&0xFF);
		tofirst[1]=(low>>8u);
		tofirst[2]=(high&0xFF);
		tofirst[3]=(high>>8u);
	}
	fromfirst+=sizeof(val);
	tofirst+=sizeof(val);
	--ndiff2;
}
while(ndiff2);
if(!ndiff2)
{
	break;
}
