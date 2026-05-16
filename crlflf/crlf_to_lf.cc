#include<fast_io.h>
#include<fast_io_device.h>
#include<fast_io_dsal/string_view.h>
#include<fast_io_dsal/string.h>
#include<memory>

int main(int argc, char** argv)
{
    using namespace ::fast_io::iomnp;
    if(argc<2)
    {
        if(argc!=0)
        {
            perr("Usage: ",os_c_str(*argv)," <dir>\n");
        }
        return 1;
    }
    ::fast_io::dir_file dir(os_c_str(argv[1]));
    for(
        ::fast_io::u8string buffer;
        auto ent : recursive(at(dir)))
    {
        ::fast_io::u8string_view ext(u8extension(ent));
        if (ext == u8".h" || ext == u8".cc" ||
            ext == u8".hpp" || ext == u8".ixx" ||
            ext == u8".cpp" || ext == u8".xsd" ||
            ext == u8".idl" || ext == u8".inl" ||
            ext == u8".cppm" || ext == u8".json")
        {
            {
                ::fast_io::u8posix_file source(drt(ent),::fast_io::open_mode::in|::fast_io::open_mode::text);
                buffer.clear();
                ::fast_io::u8ostring_ref_fast_io oref{::std::addressof(buffer)};
                ::fast_io::operations::transmit_until_eof(oref,source);
            }
            ::fast_io::u8native_file nf(drt(ent),::fast_io::open_mode::out);
            print(nf,buffer);
        }
    }
}