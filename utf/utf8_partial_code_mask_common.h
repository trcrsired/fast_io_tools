auto lengthm2{length-2};
auto [mask1,mask2,mask3]=utf8masks[lengthm2];
if((val&mask1)==mask2)
{
	val&=mask3;
	val>>=(static_cast<unsigned>(2-lengthm2)<<3);
	val=(val&0xFF)|
		((val&0xFF00)>>2)|
		((val&0xFF0000)>>4)|
		((val&0xFF000000)>>6);
}
else
{
	val=0xFFFD;
}
