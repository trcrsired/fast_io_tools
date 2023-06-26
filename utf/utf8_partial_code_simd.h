constexpr
	::std::size_t ndiffmask{static_cast<::std::size_t>(N-1u)};
constexpr
	int ndiffshift{::std::bit_width(ndiffmask)};
::std::size_t ndiff{(fromdiff>>ndiffshift)+((fromdiff&ndiffmask)!=0u)};
do
{
	simvec.load(fromfirst);
	res=((simvec&cmp128)==cmp128);
	if(!is_all_zeros(res))
	{
		break;
	}
	if constexpr(N==16)
	{
		ret.value=__builtin_shufflevector(simvec.value,zeros.value,0,16,16,16,1,16,16,16,
			2,16,16,16,3,16,16,16);
		ret.store(tofirst);
		ret.value=__builtin_shufflevector(simvec.value,zeros.value,4,16,16,16,5,16,16,16,
			6,16,16,16,7,16,16,16);
		ret.store(tofirst+4);
		ret.value=__builtin_shufflevector(simvec.value,zeros.value,8,16,16,16,9,16,16,16,
			10,16,16,16,11,16,16,16);
		ret.store(tofirst+8);
		ret.value=__builtin_shufflevector(simvec.value,zeros.value,12,16,16,16,13,16,16,16,
			14,16,16,16,15,16,16,16);
		ret.store(tofirst+12);
	}
	else if constexpr(N==32)
	{
		ret.value=__builtin_shufflevector(simvec.value,zeros.value,0,32,32,32,1,32,32,32,2,32,32,32,3,32,32,32,4,32,32,32,5,32,32,32,6,32,32,32,7,32,32,32);
		ret.store(tofirst);
		ret.value=__builtin_shufflevector(simvec.value,zeros.value,8,32,32,32,9,32,32,32,10,32,32,32,11,32,32,32,12,32,32,32,13,32,32,32,14,32,32,32,15,32,32,32);
		ret.store(tofirst+8);
		ret.value=__builtin_shufflevector(simvec.value,zeros.value,16,32,32,32,17,32,32,32,18,32,32,32,19,32,32,32,20,32,32,32,21,32,32,32,22,32,32,32,23,32,32,32);
		ret.store(tofirst+16);
		ret.value=__builtin_shufflevector(simvec.value,zeros.value,24,32,32,32,25,32,32,32,26,32,32,32,27,32,32,32,28,32,32,32,29,32,32,32,30,32,32,32,31,32,32,32);
		ret.store(tofirst+24);
	}
	else if constexpr(N==64)
	{
		ret.value=__builtin_shufflevector(simvec.value,zeros.value,0,64,64,64,1,64,64,64,2,64,64,64,3,64,64,64,4,64,64,64,5,64,64,64,6,64,64,64,7,64,64,64,8,64,64,64,9,64,64,64,10,64,64,64,11,64,64,64,12,64,64,64,13,64,64,64,14,64,64,64,15,64,64,64);
		ret.store(tofirst);
		ret.value=__builtin_shufflevector(simvec.value,zeros.value,16,64,64,64,17,64,64,64,18,64,64,64,19,64,64,64,20,64,64,64,21,64,64,64,22,64,64,64,23,64,64,64,24,64,64,64,25,64,64,64,26,64,64,64,27,64,64,64,28,64,64,64,29,64,64,64,30,64,64,64,31,64,64,64);
		ret.store(tofirst+16);
		ret.value=__builtin_shufflevector(simvec.value,zeros.value,32,64,64,64,33,64,64,64,34,64,64,64,35,64,64,64,36,64,64,64,37,64,64,64,38,64,64,64,39,64,64,64,40,64,64,64,41,64,64,64,42,64,64,64,43,64,64,64,44,64,64,64,45,64,64,64,46,64,64,64,47,64,64,64);
		ret.store(tofirst+32);
		ret.value=__builtin_shufflevector(simvec.value,zeros.value,48,64,64,64,49,64,64,64,50,64,64,64,51,64,64,64,52,64,64,64,53,64,64,64,54,64,64,64,55,64,64,64,56,64,64,64,57,64,64,64,58,64,64,64,59,64,64,64,60,64,64,64,61,64,64,64,62,64,64,64,63,64,64,64);
		ret.store(tofirst+48);
	}
	fromfirst+=N;
	tofirst+=N;
	--ndiff;
}
while(ndiff);
if(!ndiff)
{
	break;
}
