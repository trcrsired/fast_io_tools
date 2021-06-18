#pragma once

namespace pe_file_format
{

struct
#if __has_cpp_attribute(gnu::may_alias)
[[gnu::may_alias]]
#endif
image_dos_header
{
std::uint16_t   e_magic;                     // Magic number
std::uint16_t   e_cblp;                      // Bytes on last page of file
std::uint16_t   e_cp;                        // Pages in file
std::uint16_t   e_crlc;                      // Relocations
std::uint16_t   e_cparhdr;                   // Size of header in paragraphs
std::uint16_t   e_minalloc;                  // Minimum extra paragraphs needed
std::uint16_t   e_maxalloc;                  // Maximum extra paragraphs needed
std::uint16_t   e_ss;                        // Initial (relative) SS value
std::uint16_t   e_sp;                        // Initial SP value
std::uint16_t   e_csum;                      // Checksum
std::uint16_t   e_ip;                        // Initial IP value
std::uint16_t   e_cs;                        // Initial (relative) CS value
std::uint16_t   e_lfarlc;                    // File address of relocation table
std::uint16_t   e_ovno;                      // Overlay number
std::uint16_t   e_res[4];                    // Reserved words
std::uint16_t   e_oemid;                     // OEM identifier (for e_oeminfo)
std::uint16_t   e_oeminfo;                   // OEM information; e_oemid specific
std::uint16_t   e_res2[10];                  // Reserved words
std::uint32_t   e_lfanew;                    // File address of new exe header
};

}
