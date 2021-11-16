#pragma once

/*
https://github.com/vkandy/jenkins-hash-java/blob/master/src/JenkinsHash.java
*/

namespace fast_io
{

namespace details
{

template<std::unsigned_integral return_type>
requires (std::same_as<return_type,std::uint32_t>||std::same_as<return_type,std::uint64_t>)
inline constexpr return_type jenkins_hash_impl(::fast_io::freestanding::string_view k,std::uint32_t pc,[[maybe_unused]] std::uint32_t pb) noexcept
{
	std::uint32_t a{0xdeadbeef + static_cast<std::uint32_t>(k.size()) + pc};
	std::uint32_t b{a},c{a};
	if constexpr(std::same_as<return_type,std::uint64_t>)
		c+=pb;
	auto i{k.data()};
	for(auto end{k.data()+k.size()};i+12<end;i+=12)
	{
		if (std::is_constant_evaluated())
		{
			a += static_cast<std::uint32_t>(static_cast<char8_t>(*i));
			a += static_cast<std::uint32_t>(static_cast<char8_t>(i[1])) << 8;
			a += static_cast<std::uint32_t>(static_cast<char8_t>(i[2])) << 16;
			a += static_cast<std::uint32_t>(static_cast<char8_t>(i[3])) << 24;
			b += static_cast<std::uint32_t>(static_cast<char8_t>(i[4]));
			b += static_cast<std::uint32_t>(static_cast<char8_t>(i[5])) << 8;
			b += static_cast<std::uint32_t>(static_cast<char8_t>(i[6])) << 16;
			b += static_cast<std::uint32_t>(static_cast<char8_t>(i[7])) << 24;
			c += static_cast<std::uint32_t>(static_cast<char8_t>(i[8]));
			c += static_cast<std::uint32_t>(static_cast<char8_t>(i[9])) << 8;
			c += static_cast<std::uint32_t>(static_cast<char8_t>(i[10])) << 16;
			c += static_cast<std::uint32_t>(static_cast<char8_t>(i[11])) << 24;
		}
		else
		{
			std::uint32_t a1;
			::fast_io::details::my_memcpy(__builtin_addressof(a1),i,4);
			a+=a1;
			std::uint32_t b1;
			::fast_io::details::my_memcpy(__builtin_addressof(b1),i+4,4);
			b+=b1;
			std::uint32_t c1;
			::fast_io::details::my_memcpy(__builtin_addressof(c1),i+8,4);
			c+=c1;
		}
		a -= c;
		a ^= std::rotl(c, 4);
		c += b;
		b -= a;
		b ^= std::rotl(a, 6);
		a += c;
		c -= b;
		c ^= std::rotl(b, 8);
		b += a;
		a -= c;
		a ^= std::rotl(c, 16);
		c += b;
		b -= a;
		b ^= std::rotl(a, 19);
		a += c;
		c -= b;
		c ^= std::rotl(b, 4);
		b += a;
	}
	switch(::fast_io::freestanding::to_address(k.end())-i)
	{
		case 12:
			c += static_cast<std::uint32_t>(static_cast<char8_t>(i[11])) << 24;
			[[fallthrough]];
		case 11:
			c += static_cast<std::uint32_t>(static_cast<char8_t>(i[10])) << 16;
			[[fallthrough]];
		case 10:
			c += static_cast<std::uint32_t>(static_cast<char8_t>(i[9])) << 8;
			[[fallthrough]];
		case 9:
			c += static_cast<std::uint32_t>(static_cast<char8_t>(i[8]));
			[[fallthrough]];
		case 8:
			b += static_cast<std::uint32_t>(static_cast<char8_t>(i[7])) << 24;
			[[fallthrough]];
		case 7:
			b += static_cast<std::uint32_t>(static_cast<char8_t>(i[6])) << 16;
			[[fallthrough]];
		case 6:
			b += static_cast<std::uint32_t>(static_cast<char8_t>(i[5])) << 8;
			[[fallthrough]];
		case 5:
			b += static_cast<std::uint32_t>(static_cast<char8_t>(i[4]));
			[[fallthrough]];
		case 4:
			a += static_cast<std::uint32_t>(static_cast<char8_t>(i[3])) << 24;
			[[fallthrough]];
		case 3:
			a += static_cast<std::uint32_t>(static_cast<char8_t>(i[2])) << 16;
			[[fallthrough]];
		case 2:
			a += static_cast<std::uint32_t>(static_cast<char8_t>(i[1])) << 8;
			[[fallthrough]];
		case 1:
			a += static_cast<std::uint32_t>(static_cast<char8_t>(*i));
			break;
		case 0:
			if constexpr(std::same_as<return_type,std::uint32_t>)
				return c;
			else
				return (static_cast<std::uint64_t>(c)<<32)|(static_cast<std::uint64_t>(b)&0xFFFFFFFFL);
	}
	c ^= b;
	c -= std::rotl(b, 14);
	a ^= c;
	a -= std::rotl(c, 11);
	b ^= a;
	b -= std::rotl(a, 25);
	c ^= b;
	c -= std::rotl(b, 16);
	a ^= c;
	a -= std::rotl(c, 4);
	b ^= a;
	b -= std::rotl(a, 14);
	c ^= b;
	c -= std::rotl(b, 24);
	if constexpr(std::same_as<return_type,std::uint32_t>)
		return c;
	else
		return (static_cast<std::uint64_t>(c)<<32)|(static_cast<std::uint64_t>(b)&0xFFFFFFFFL);
}

}

inline constexpr std::uint32_t jenkins_hash(::fast_io::freestanding::string_view k,std::uint32_t pc) noexcept
{
	return details::jenkins_hash_impl<std::uint32_t>(k,pc,0);
}

inline constexpr std::uint64_t jenkins_hash(::fast_io::freestanding::string_view k,std::uint32_t pc,std::uint32_t pb) noexcept
{
	return details::jenkins_hash_impl<std::uint64_t>(k,pc,pb);
}

inline constexpr std::uint32_t jenkins_hash32(::fast_io::freestanding::string_view k) noexcept
{
	return details::jenkins_hash_impl<std::uint32_t>(k,0,0);
}

inline constexpr std::uint64_t jenkins_hash64(::fast_io::freestanding::string_view k) noexcept
{
	return details::jenkins_hash_impl<std::uint64_t>(k,0,0);
}

}