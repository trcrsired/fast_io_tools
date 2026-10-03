// locale_emit — compile every glibc localedata file into flat binary
// locale blobs (name.utf8.bin / name.utf16.bin / name.utf32.bin).
//
// usage: locale_emit <localedata dir> <output dir>

#include <herbceptions/error>
#include <fast_io.h>
#include <fast_io_device.h>
#include <fast_io_dsal/vector.h>
#include <fast_io_dsal/string.h>
#include <fast_io_dsal/string_view.h>
#include "localedef.h"

int main(int argc, char **argv) try
{
	using namespace ::fast_io_i18n;
	if (argc != 3)
	{
		::fast_io::perrln("usage: locale_emit <localedata dir> <output dir>");
		return 1;
	}
	::fast_io::dir_file df(::fast_io::mnp::os_c_str(argv[1]));
	::fast_io::dir_file outdir(::fast_io::mnp::os_c_str(argv[2]));
	localedef::file_cache cache;
	for (auto ent : ::fast_io::current(::fast_io::at(df)))
	{
		auto fn{::fast_io::u8filename(ent)};
		::std::u8string_view uname{fn.c_str(), fn.c_str() + fn.n};
		// file name as char string for parsing/writing
		::fast_io::string name;
		name.append(reinterpret_cast<char const *>(fn.c_str()), fn.n);
		::std::string_view nsv{name.data(), name.size()};
		if (nsv == "." || nsv == ".." || nsv == "cns11643_stroke" ||
		    nsv == "i18n_ctype" || nsv.substr(0, 8) == "iso14651" ||
		    nsv.substr(0, 8) == "translit")
		{
			continue;
		}
		try
		{
			::fast_io::println("parse ", nsv);
			localedef::lc_file_data d;
			localedef::parse_file(df, name, d, cache, 0);
			lcblob::cat_src cats[lcblob::cat_count]{};
			localedef::to_cats(d, cats, {{name.data(), name.size()}, 0});
			for (auto cs : {lcblob::blob_charset::utf8, lcblob::blob_charset::utf16,
					lcblob::blob_charset::utf32})
			{
				auto blob{lcblob::build_blob(
					cats, uname, cs, {name.data(), name.size()})};
				::fast_io::string outname{name};
				outname.append(cs == lcblob::blob_charset::utf8   ? ".utf8.bin"
						   : cs == lcblob::blob_charset::utf16 ? ".utf16.bin"
										     : ".utf32.bin",
						   cs == lcblob::blob_charset::utf32 ? 10 : 9);
				::fast_io::obuf_file of{
					::fast_io::at(outdir),
					::fast_io::mnp::os_c_str(outname.c_str())};
				::fast_io::print(of,
						 ::std::string_view{blob.data(), blob.size()});
			}
		}
		catch throws(::std::error e)
		{
			::fast_io::perrln("  ^^ on ", nsv, ": ", e);
		}
	}
	return 0;
}
catch throws(::std::error e)
{
	::fast_io::perrln("err: ", e);
	return 1;
}
