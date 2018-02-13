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
