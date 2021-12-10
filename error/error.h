#pragma once

namespace fast_io
{

struct error
{
	using value_type = std::common_type_t<std::uintptr_t,std::uint_least32_t>;
	value_type domain{};
	value_type code{};
};

inline constexpr bool operator==(error e1,error e2) noexcept
{
	return e1.domain==e2.domain&&e1.code==e2.code;
}

inline constexpr bool operator!=(error e1,error e2) noexcept
{
	return !(e1==e2);
}

template<typename T>
struct error_domain_type_t
{
explicit constexpr error_domain_type_t() noexcept = default;
};

template<typename T>
inline constexpr error_domain_type_t<T> error_domain_type{};

template<typename T>
concept error_domain = std::is_trivially_copyable_v<T> && sizeof(T)<=sizeof(std::uintptr_t) &&
	requires(error e,T t)
{
	{error_domain_define(error_domain_type<T>)}->std::same_as<std::uintptr_t>;
	{error_equivalent_define(error_domain_type<T>,e,t)}->std::same_as<bool>;
};

template<error_domain D>
inline constexpr bool is_domain(error e) noexcept
{
	constexpr std::uintptr_t v{error_domain_define(error_domain_type<D>)};
	return v==e.domain;
}

template<error_domain D>
inline constexpr bool operator==(error e,D t) noexcept
{
	return is_domain<D>(e)&&(static_cast<std::uintptr_t>(t)==e.code);
}

template<error_domain D>
inline constexpr bool operator==(D t,error e) noexcept
{
	return error_equivalent_define(error_domain_type<D>,e,t);
}

template<error_domain D>
inline constexpr bool operator!=(error e,D t) noexcept
{
	return !error_equivalent_define(error_domain_type<D>,e,t);
}

template<error_domain D>
inline constexpr bool operator!=(D t,error e) noexcept
{
	return !error_equivalent_define(error_domain_type<D>,e,t);
}

}
