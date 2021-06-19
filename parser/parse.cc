#include<fast_io.h>
#include"dos_header.h"
#include"pe_file.h"

int main(int argc,char** argv)
{
	if(argc<2)
	{
		perr("Usage: ",fast_io::mnp::chvw(argv[0])," <pe executable>\n");
		return 1;
	}
	fast_io::native_file_loader loader(argv[1]);
	pe_file_format::pe_file pe{pe_file_format::deserialize(loader.begin(),loader.end())};
	println("PE header magic:",pe.peheader.mMagic,
		"\nmachine type:",fast_io::mnp::hex(pe.peheader.mMachine),
		"\noptionalheader magic:",pe.peoptionalheader.mMagic,
		"\nmajor operating system version:",pe.peoptionalheader.mMajorOperatingSystemVersion,
		"\nminor operating system version:",pe.peoptionalheader.mMinorOperatingSystemVersion,
		"\nmajor subsystem version:",pe.peoptionalheader.mMajorSubsystemVersion,
		"\nminor subsystem version:",pe.peoptionalheader.mMinorSubsystemVersion,
		"\ncheck sum:",pe.peoptionalheader.mCheckSum);
}
