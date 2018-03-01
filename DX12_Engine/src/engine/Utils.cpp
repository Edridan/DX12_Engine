#include "Utils.h"

#include <sstream>
#include <fstream>
#include <codecvt>

#include <windows.h>
#include <Shlwapi.h>

void Files::FileToWStr(std::wstring & o_Out, const char * i_Filename)
{
	std::wifstream wif(i_Filename);
	wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
	
	std::wstringstream wss;

	wss << wif.rdbuf();
	o_Out = wss.str();
}

FORCEINLINE std::string Files::ConvertToWinPath(const std::string & i_Filepath)
{
	return String::ReplaceAll(i_Filepath, "/", "\\");
}

std::wstring Files::ConvertToWinPath(const std::wstring & i_Filepath)
{
	return String::ReplaceAll(i_Filepath, L"/", L"\\");
}

bool Files::FileExist(const std::string & i_Filepath)
{
	std::string filepath = Files::ConvertToWinPath(i_Filepath);
	//return PathFileExists(filepath.c_str());
	return false;
}

bool Files::FileExist(const std::wstring & i_Filepath)
{
	std::wstring filepath = Files::ConvertToWinPath(i_Filepath);
	//return PathFileExists(filepath.c_str());
	return false;
}


void Files::GetFilesInFolder(std::vector<std::string>& o_Files, const std::string & i_Folder, const std::string & i_Filetype, bool i_ReturnFolderInFiles)
{
	WIN32_FIND_DATA data;
	std::wstring folder;
	String::Utf8ToUtf16(folder, i_Folder);

	folder = String::ReplaceAll(folder, L"/", L"\\");
	folder = L".\\" + folder;
	folder.append(L"\\*");
	HANDLE hFind = FindFirstFile(folder.c_str(), &data);      // DIRECTORY

	if (hFind != INVALID_HANDLE_VALUE) 
	{
		do 
		{
			std::string file;

			if (i_ReturnFolderInFiles)
			{
				String::Utf16ToUtf8(file, data.cFileName);
				file = i_Folder + "/" + file;
			}
			else
				String::Utf16ToUtf8(file, data.cFileName);

			if (i_Filetype.size() != 0)
			{
				if (!String::EndWith(file, i_Filetype))	continue;
			}

			o_Files.push_back(file);
		} while (FindNextFile(hFind, &data));

		FindClose(hFind);
	}
}

void Files::GetFilesInFolder(std::vector<std::wstring>& o_Files, const std::wstring & i_Folder, const std::wstring & i_Filetype, bool i_ReturnFolderInFiles)
{
	WIN32_FIND_DATA data;
	std::wstring folder = L".\\" + String::ReplaceAll(i_Folder, L"/", L"\\") + L"\\*";
	HANDLE hFind = FindFirstFile(i_Folder.c_str(), &data);      // DIRECTORY

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			if (i_Filetype.size() != 0)
			{
				if (!String::EndWith(data.cFileName, i_Filetype))	continue;
			}

			if (i_ReturnFolderInFiles)
				o_Files.push_back(i_Folder + L"/" + data.cFileName);
			else
				o_Files.push_back(data.cFileName);

		} while (FindNextFile(hFind, &data));

		FindClose(hFind);
	}
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

std::string String::ReplaceAll(const std::string & i_String, const std::string & i_From, const std::string & i_To)
{
	size_t startPos = 0;
	std::string str = i_String;
	while ((startPos = str.find(i_From, startPos)) != std::string::npos)
	{
		str.replace(startPos, i_From.length(), i_To);
		startPos += i_To.length(); // Handles case where 'to' is a substring of 'from'
	}

	return str;
}

std::wstring String::ReplaceAll(const std::wstring & i_String, const std::wstring & i_From, const std::wstring & i_To)
{
	size_t startPos = 0;
	std::wstring str = i_String;
	while ((startPos = str.find(i_From, startPos)) != std::string::npos)
	{
		str.replace(startPos, i_From.length(), i_To);
		startPos += i_To.length(); // Handles case where 'to' is a substring of 'from'
	}

	return str;
}

std::string String::IntToString(int i_Value)
{
	char buffer[32];
	_itoa_s(i_Value, buffer, 10);

	return std::string(buffer);
}

std::string String::Int64ToString(long int i_Value)
{
	char buffer[64];
	_ltoa_s(i_Value, buffer, 10);

	return std::string(buffer);
}

std::string String::UInt64ToString(unsigned long int i_Value)
{
	char buffer[64];
	_ultoa_s(i_Value, buffer, 10);

	return std::string(buffer);
}

bool String::EndWith(const std::string & i_String, const std::string & i_End)
{
	if (i_End.size() > i_String.size()) return false;
	return std::equal(i_End.rbegin(), i_End.rend(), i_String.rbegin());
}

bool String::EndWith(const std::wstring & i_String, const std::wstring & i_End)
{
	if (i_End.size() > i_String.size()) return false;
	return std::equal(i_End.rbegin(), i_End.rend(), i_String.rbegin());
}

bool String::StartWith(const std::string & i_String, const std::string & i_Start)
{
	return (i_String.substr(0, i_Start.size()) == i_Start);
}

bool String::StartWith(const std::wstring & i_String, const std::wstring & i_Start)
{
	return (i_String.substr(0, i_Start.size()) == i_Start);
}
