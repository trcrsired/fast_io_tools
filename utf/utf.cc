#include<fast_io.h>
using namespace fast_io::io;
#include"utf.h"

int main()
{
	char8_t buffer[]=u8"😀 😃 😄 😁 😆 12348724937149127598057193085732850 😅 😂 🤣 🥲 🥹 ☺️ 😊 😇 🙂 🙃 😉 😌 😍 🥰 😘 😗 😙 😚 😋 😛 😝 😜 🤪 🤨 🧐 🤓 😎 🥸 🤩 🥳 😏 😒 😞 😔 😟 😕 🙁 ☹️ 😣 😖 😫 😩 🥺 😢 😭 😮‍💨 😤 😠 😡 🤬 🤯 😳 🥵 🥶 😱 😨 😰 😥 😓 🫣 🤗 🫡 🤔 🫢 🤭 🤫 🤥 😶 😶‍🌫️ 😐 😑 😬 🫨 🫠 🙄 😯 😦 😧 😮 😲 🥱 😴 🤤 😪 😵 😵‍💫 🫥 🤐 🥴 🤢 🤮 🤧 😷";

//	char8_t buffer[]=u8"123487249371491275980571930857328507130295731295sdhfsdhaiosdghdsoighsiodg";

	char32_t buffer32[1000]{};
	auto [it,dit]=::fast_io::details::utf8_to_utf32_impl(buffer,buffer+std::ranges::size(buffer)-1,buffer32,buffer32+std::ranges::size(buffer32));

	::fast_io::io::println(dit-buffer32,"\n",::fast_io::mnp::code_cvt(::fast_io::mnp::strvw(buffer32,dit)));
}
