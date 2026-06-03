#pragma once


namespace fast_io
{

namespace details
{

inline constexpr std::size_t biguint_assign_add_assign_single(
    std::size_t* ap, std::size_t an,
    std::size_t const* bp, std::size_t bn) noexcept
{
    bool carry{};

    // 1. Add overlapping limbs
    std::size_t mn{bn < an ? bn : an};

    for(std::size_t i{}; i != mn; ++i)
    {
        auto& ai{ap[i]};
        ai = ::fast_io::intrinsics::addc(ai, bp[i], carry, carry);
    }


    constexpr ::std::size_t zero{};
    // 2. If a is exhausted but b still has limbs
    if(an < bn)
    {
        for(std::size_t i{an}; i != bn; ++i)
        {
            ap[i] = ::fast_io::intrinsics::addc(zero, bp[i], carry, carry);
        }
        an = bn;
    }
    // 3. b is exhausted but carry still propagates through a
    std::size_t i{mn};
    for(; carry && i != an; ++i)
    {
        auto& ai{ap[i]};
        ai = ::fast_io::intrinsics::addc(ai, zero, carry, carry);
    }

    // 4. If carry still remains, append one more limb
    if(i==an && carry)
    {
        ap[an] = 1u;
	++an;
    }

    return an;
}


inline constexpr std::size_t biguint_assign_x2_single(
    std::size_t* ap, std::size_t n) noexcept
{
    bool carry{};
    auto i{ap},e{ap + n};
    for(; i != e; ++i)
    {
	::std::size_t const val{*i};
        *i = ::fast_io::intrinsics::addc(val, val, carry, carry);
    }
    if (carry)
    {
	*i = 1u;
	++n;
    }
    return n;
}

inline constexpr std::size_t biguint_assign_mul(
    std::size_t* ap, std::size_t n, ::std::size_t val) noexcept
{
	constexpr
		::std::size_t zero{};
	bool carryadd;
	::std::size_t high;
	::std::size_t carry{};
	auto i{ap},e{ap + n};
	for(; i != e; ++i)
	{
		::std::size_t const di{*i};
		::std::size_t low{::fast_io::intrinsics::umul(di, val, high)};
		*i = ::fast_io::intrinsics::addc(low, carry, false, carryadd);
		carry = ::fast_io::intrinsics::addc(high, zero, carryadd, carryadd);
	}
	if (carry)
	{
		*i = carry;
		++n;
	}
	return n;
}

inline constexpr ::std::size_t biguint_assign_adds_mul_constant_impl(::std::size_t* valp, ::std::size_t valn, ::std::size_t count) noexcept
{
	switch(count)
	{
		case 0u:
		{
			return valn;
		}
		case 1u:
		{
			return ::fast_io::details::biguint_assign_x2_single(valp, valn);
		}
		default:
		{
			return ::fast_io::details::biguint_assign_mul(valp, valn, count + 1u);
		}
	}
}

template<typename Alloc>
inline constexpr void biguint_assign_adds_impl(::fast_io::containers::vector<::std::size_t, Alloc>& ref, ::fast_io::basic_io_scatter_t<::std::size_t> const* scattersp, ::std::size_t scattersn) noexcept
{
	::std::size_t self_added_count{};
	::std::size_t to_allocate_n{};
	::std::size_t ton{};
	auto refdata{ref.data()};
	for(auto i{scattersp},e{scattersn+n};i!=e;++i)
	{
		if(refdata == i->base)
		{
			++self_added_count;
		}
		if(__builtin_add_overflow(to_allocate_n, i->len, __builtin_addressof(to_allocate_n)))
		{
			::fast_io::fast_terminate();
		}
	}

	constexpr
		::std::size_t mx{::std::numeric_limits<::std::size_t>::max()};
	if(self_added_count == mx) [[unlikely]]
	{
		::fast_io::fast_terminate();
	}
	ref.reserve(to_allocate_n);
	auto newrefdata{ref.imp.begin_ptr};
	::std::size_t const newsz{static_cast<::std::size_t>(ref.imp.curr_ptr - newrefdata)};
	::fast_io::details::biguint_assign_adds_mul_constant_impl(newrefdata, newsz, count);
	::std::size_t sn{};
	for(auto i{scattersp},e{scattersn+n};i!=e;++i)
	{
		auto base{i->base};
		if(refdata == base)
		{
			continue;
		}
		newsz = ::fast_io::details::biguint_assign_add_assign_single(newrefdata, newsz, base, i->len);
	}
	ref.imp.curr_ptr = newrefdata + newsz;
}

}

namespace manipulators
{

template<typename vec_type>
struct biguint_ref
{
	using vector_type = vec_type;
	using reference_type = vector_type*;
	using manip_tag_t = ::fast_io::manip_tag;
	reference_type ptr{};
}

template<typename Alloc>
inline constexpr ::fast_io::manipulators::biguint_ref<::fast_io::containers::vector<::std::size_t, Alloc>> as_biguint_ref(::fast_io::containers::vector<::std::size_t, Alloc>& ref) noexcept
{
	return {__builtin_addressof(ref)};
}

}

namespace containers
{

template<typename Alloc>
class basic_biguint
{
public:
	using allocator = Alloc;
	using size_type = ::std::size_t;
	using value_type = size_type;
	using difference_type = ::std::ptrdiff_t;
	using pointer = value_type*;
	using const_pointer = value_type const*;
	using vector_type = ::fast_io::containers::vector<::std::size_t, Alloc>;
	vector_type rep;

	constexpr vector_type const& get_vector() const noexcept
	{
		return rep;
	}
	constexpr vector_type& get_vector() noexcept
	{
		return rep;
	}

	constexpr ::fast_io::basic_io_scatter_t<::std::size_t> as_scatter() const noexcept
	{
		return {rep.data(), rep.size()};
	}
};

template<typename Alloc>
inline constexpr void biguint_ref_assign_adds(::fast_io::manipulators::biguint_ref<::fast_io::containers::vector<::std::size_t, Alloc>> dest,
	::fast_io::basic_io_scatter_t<::fast_io::basic_io_scatter_t<::std::size_t>> scatters) noexcept
{
	::fast_io::details::biguint_assign_adds_impl(*dest.ptr, scatters.base, scatters.len);
}

}

}
