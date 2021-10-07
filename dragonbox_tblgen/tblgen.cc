#define FAST_IO_DISABLE_FLOATING_POINT
#include<fast_io.h>
#include"dragonbox.h"

int main()
{


	auto const& tb{jkj::dragonbox::detail::div::table_holder<std::uint32_t, 5 , jkj::dragonbox::detail::impl<float>::max_power_of_factor_of_5+2>::table};

	using namespace fast_io::mnp;
	println("tb size:",tb.size);
	for(std::size_t i{};i!=tb.size;++i)
		print("{",hexupper<true,true>(tb.mod_inv[i]),"u,",hexupper<true,true>(tb.max_quotients[i]),"u},\n");
//	println(jkj::dragonbox::detail::impl<float>::max_power_of_factor_of_5);
//	jkj::dragonbox::to_decimal(3.8678793e+37f);
}