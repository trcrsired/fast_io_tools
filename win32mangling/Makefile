all:manglingx86_64.s manglingi686.s manglingarm.s nt_manglingx86_64.s nt_manglingi686.s nt_manglingarm.s msvc_linker_64.h msvc_linker_32_i686.h msvc_linker_32.h

manglingx86_64.s:mangling.cc
	clang++ --target=x86_64-windows-msvc -S mangling.cc -Ofast -std=c++20 -o manglingx86_64.s -Wno-deprecated-declarations -Wall -Wextra -Wpedantic
manglingi686.s:mangling.cc
	clang++ --target=i686-windows-msvc -S mangling.cc -Ofast -std=c++20 -o manglingi686.s -Wno-deprecated-declarations -Wall -Wextra -Wpedantic
manglingarm.s:mangling.cc
	clang++ --target=arm-windows-msvc -S mangling.cc -Ofast -std=c++20 -o manglingarm.s -Wno-deprecated-declarations -Wall -Wextra -Wpedantic
nt_manglingx86_64.s:nt_mangling.cc
	clang++ --target=x86_64-windows-msvc -S nt_mangling.cc -Ofast -std=c++20 -o nt_manglingx86_64.s -Wno-deprecated-declarations -Wall -Wextra -Wpedantic
nt_manglingi686.s:nt_mangling.cc
	clang++ --target=i686-windows-msvc -S nt_mangling.cc -Ofast -std=c++20 -o nt_manglingi686.s -Wno-deprecated-declarations -Wall -Wextra -Wpedantic
nt_manglingarm.s:nt_mangling.cc
	clang++ --target=arm-windows-msvc -S nt_mangling.cc -Ofast -std=c++20 -o nt_manglingarm.s -Wno-deprecated-declarations -Wall -Wextra -Wpedantic


msvc_linker_64.h:x86_64.txt
	./msvc_comment x86_64.txt msvc_linker_64.h
msvc_linker_32_i686.h:i686.txt
	./msvc_comment i686.txt msvc_linker_32_i686.h
msvc_linker_32.h:arm.txt
	./msvc_comment arm.txt msvc_linker_32.h
clean:
	del msvc_linker_32.h msvc_linker_32_i686.h msvc_linker_64.h manglingarm.s manglingi686.s manglingx86_64.s nt_manglingx86_64.s nt_manglingi686.s nt_manglingarm.s