all:manglingx86_64.s manglingi686.s manglingarm.s nt_manglingx86_64.s nt_manglingi686.s nt_manglingarm.s msvc_linker_64.h msvc_linker_32_i686.h msvc_linker_32.h msvc_comment

manglingx86_64.s:mangling.cc
	clang++ --target=x86_64-windows-msvc -S mangling.cc -O3 -std=c++26 -o manglingx86_64.s -Wno-deprecated-declarations -Wall -Wextra -Wpedantic --sysroot=${SYSROOT}
manglingi686.s:mangling.cc
	clang++ --target=i686-windows-msvc -S mangling.cc -O3 -std=c++26 -o manglingi686.s -Wno-deprecated-declarations -Wall -Wextra -Wpedantic --sysroot=${SYSROOT}
manglingarm.s:mangling.cc
	clang++ --target=arm-windows-msvc -S mangling.cc -O3 -std=c++26 -o manglingarm.s -Wno-deprecated-declarations -Wall -Wextra -Wpedantic --sysroot=${SYSROOT}
nt_manglingx86_64.s:nt_mangling.cc
	clang++ --target=x86_64-windows-msvc -S nt_mangling.cc -O3 -std=c++26 -o nt_manglingx86_64.s -Wno-deprecated-declarations -Wall -Wextra -Wpedantic --sysroot=${SYSROOT}
nt_manglingi686.s:nt_mangling.cc
	clang++ --target=i686-windows-msvc -S nt_mangling.cc -O3 -std=c++26 -o nt_manglingi686.s -Wno-deprecated-declarations -Wall -Wextra -Wpedantic --sysroot=${SYSROOT}
nt_manglingarm.s:nt_mangling.cc
	clang++ --target=arm-windows-msvc -S nt_mangling.cc -O3 -std=c++26 -o nt_manglingarm.s -Wno-deprecated-declarations -Wall -Wextra -Wpedantic --sysroot=${SYSROOT}
msvc_comment:msvc_comment.cc
	clang++ -o msvc_comment msvc_comment.cc -O3 -std=c++26 -Wno-deprecated-declarations -Wall -Wextra -Wpedantic -I{FAST_IO_DIR}\include

msvc_linker_64.h:msvc_comment x86_64.txt
	./msvc_comment x86_64.txt msvc_linker_64.h
msvc_linker_32_i686.h:msvc_comment i686.txt
	./msvc_comment i686.txt msvc_linker_32_i686.h
msvc_linker_32.h:msvc_comment arm.txt
	./msvc_comment arm.txt msvc_linker_32.h
clean:
	rm msvc_comment msvc_linker_32.h msvc_linker_32_i686.h msvc_linker_64.h manglingarm.s manglingi686.s manglingx86_64.s nt_manglingx86_64.s nt_manglingi686.s nt_manglingarm.s