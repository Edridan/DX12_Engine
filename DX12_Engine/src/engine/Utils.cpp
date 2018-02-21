#include "Utils.h"

#include <sstream>
#include <fstream>
#include <codecvt>

void Files::FileToWStr(std::wstring & o_Out, const char * i_Filename)
{
	std::wifstream wif(i_Filename);
	wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
	
	std::wstringstream wss;

	wss << wif.rdbuf();
	o_Out = wss.str();
}

void String::Utf16ToUtf8(std::string & o_Out, const std::wstring & i_String)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	o_Out = conv.to_bytes(i_String);
}

void String::Utf8ToUtf16(std::wstring & o_Out, const std::string & i_String)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
	o_Out = conv.from_bytes(i_String);
}
