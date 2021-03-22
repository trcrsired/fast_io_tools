all:manglingx86_64.s manglingi686.s manglingarm.s msvc_linker_64.h msvc_linker_32_i686.h msvc_linker_32.h

manglingx86_64.s:mangling.cc
	clang++ --target=x86_64-windows-msvc -S mangling.cc -Ofast -std=c++20 -o manglingx86_64.s
manglingi686.s:mangling.cc
	clang++ --target=i686-windows-msvc -S mangling.cc -Ofast -std=c++20 -o manglingi686.s
manglingarm.s:mangling.cc
	clang++ --target=arm-windows-msvc -S mangling.cc -Ofast -std=c++20 -o manglingarm.s
msvc_linker_64.h:x86_64.txt
	msvc_comment x86_64.txt msvc_linker_64.h
msvc_linker_32_i686.h:i686.txt
	msvc_comment i686.txt msvc_linker_32_i686.h
msvc_linker_32.h:arm.txt
	msvc_comment arm.txt msvc_linker_32.h
clean:
	rm msvc_linker_32.h msvc_linker_32_i686.h msvc_linker_64.h manglingarm.s manglingi686.s manglingx86_64.s