#include<cstdint>
#include<type_traits>
#include"apis.h"
#define WIN32_LEAN_AND_MEAN
#include<windows.h>
#include<bcrypt.h>

#define DEF(x) auto get_##x() {return fast_io::win32::x;} auto sys_##x() {return x;}
#define DEF_FROM(x,y) auto get_##x() {return fast_io::win32::x;} auto sys_##x() {return y;}
#define DEF_FROM_C_LINKER(x,y) auto get_##x() {return fast_io::win32::x;} auto sys_##x() {return fast_io_c_linker::y;}

DEF(BCryptOpenAlgorithmProvider)
DEF(BCryptCloseAlgorithmProvider)
DEF(BCryptGetProperty)
DEF(BCryptCreateHash)
DEF(BCryptHashData)
DEF(BCryptFinishHash)
DEF(BCryptDestroyHash)