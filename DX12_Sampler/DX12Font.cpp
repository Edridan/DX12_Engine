#include "DX12Font.h"

#include "DX12Texture.h"
#include <fstream>
#include <iostream>

DX12Font::DX12Font(const wchar_t * i_fontDefFileinition)
	:m_IsLoaded(false)
{
	std::wifstream filestream;
	std::wstring fontDefFile = i_fontDefFileinition;	// location where the font def file is
	std::wstring fontTexFile;

	// retreive the folder of the font texture file
	size_t pos = fontDefFile.find_last_of(L"/") + 1;
	fontTexFile.append(fontDefFile, 0, pos);

	// open the font definition file
	filestream.open(fontDefFile.c_str());

	if (!filestream.is_open())
	{
		POPUP_ERROR("Unable to open font definition file");
		DEBUG_BREAK;
		return;
	}

	std::wstring	temp;
	size_t			startPos;

	// extract font name
	filestream >> temp >> temp;	// info face ="#####"
	startPos = temp.find(L"\"") + 1;
	m_Name = temp.substr(
		startPos,
		temp.size() - startPos - 1
	);

	// extract font size
	filestream >> temp;		// size=##
	startPos = temp.find(L"=") + 1;
	m_Size = std::stoi(
		temp.substr(startPos, temp.size() - startPos)
	);

	// bold, italic, charset, unicode, stretchH, smooth, aa, padding, spacing (not mandatory data)
	filestream >> temp >> temp >> temp >> temp >> temp >> temp >> temp;

	// extract font padding
	FLOAT padding[4] = { 0.f, 0.f, 0.f, 0.f };

	filestream >> temp;	// padding=#(up),#(right),#(down),#(left)
	startPos = temp.find(L"=") + 1;
	temp = temp.substr(startPos, temp.size() - startPos);	// #(up),#(right),#(down),#(left)

	for (UINT i = 0; i < 4; ++i)
	{
		startPos = temp.find(L",");
		padding[i] = (FLOAT)std::stoi(temp.substr(0, startPos));
		// remove the last number
		if (startPos != std::wstring::npos)
		{
			temp.substr(startPos, temp.size() - startPos);
		}
	}

	m_Padding = *(Padding*)padding;

	filestream >> temp;	// spacing=#,#

	// get lineheight (how much to move down for each line), and normalize (between 0.0 and 1.0 based on size of font)
	filestream >> temp >> temp; // common lineHeight=##
	startPos = temp.find(L"=") + 1;
	m_LineHeight = (float)std::stoi(temp.substr(startPos, temp.size() - startPos));

	// get base height (height of all characters), and normalize (between 0.0 and 1.0 based on size of font)
	filestream >> temp;	// base=##
	startPos = temp.find(L"=") + 1;
	m_BaseHeight = (float)std::stoi(temp.substr(startPos, temp.size() - startPos));

	// get texture size
	filestream >> temp;	// scaleW=##
	startPos = temp.find(L"=") + 1;
	m_TextureSize.x = std::stoi(temp.substr(startPos, temp.size() - startPos));
	
	// get texture size
	filestream >> temp;	// scaleH=##
	startPos = temp.find(L"=") + 1;
	m_TextureSize.y = std::stoi(temp.substr(startPos, temp.size() - startPos));

	// pages, packed, page id
	filestream >> temp >> temp; // pages=## packed=##
	filestream >> temp >> temp; // page id=#

	// retreive texture file
	filestream >> temp; // file="Arial.png"
	startPos = temp.find(L"\"") + 1;
	fontTexFile.append(temp, startPos, temp.size() - startPos - 1);

	// load texture for the font
	m_FontTexture = new DX12Texture(fontTexFile.c_str());

	if (!m_FontTexture->IsLoaded())
	{
		POPUP_ERROR("Unable to open font texture");
		DEBUG_BREAK;
		return;
	}

	// get number of characters
	filestream >> temp >> temp; // chars count=##
	startPos = temp.find(L"=") + 1;
	m_NumCharacters = (UINT)std::stoi(temp.substr(startPos, temp.size() - startPos));

	// list char alloc
	m_CharList = new FontChar[m_NumCharacters];

	for (UINT c = 0; c < m_NumCharacters; ++c)
	{
		FontChar & ch = m_CharList[c];

		// unicode id
		filestream >> temp >> temp;	// char id=##
		startPos = temp.find(L"=") + 1;
		ch.Id = std::stoi(temp.substr(startPos, temp.size() - startPos));

		// get x
		filestream >> temp;	// x=##
		startPos = temp.find(L"=") + 1;
		ch.U = (float)std::stoi(temp.substr(startPos, temp.size() - startPos)) / (float)m_TextureSize.x;

		// get y
		filestream >> temp;	// y=##
		startPos = temp.find(L"=") + 1;
		ch.V = (float)std::stoi(temp.substr(startPos, temp.size() - startPos)) / (float)m_TextureSize.x;

		// get width
		filestream >> temp; // width=##
		startPos = temp.find(L"=") + 1;
		temp = temp.substr(startPos, temp.size() - startPos);
		ch.Width = (float)std::stoi(temp);
		ch.tWidth = (float)std::stoi(temp) / (float)m_TextureSize.x;

		// get height
		filestream >> temp; // height=##
		startPos = temp.find(L"=") + 1;
		temp = temp.substr(startPos, temp.size() - startPos);
		ch.Height = (float)std::stoi(temp);
		ch.tHeight = (float)std::stoi(temp) / (float)m_TextureSize.y;

		// get xoffset
		filestream >> temp; // xoffset=##
		startPos = temp.find(L"=") + 1;
		ch.xOffset = (float)std::stoi(temp.substr(startPos, temp.size() - startPos));

		// get yoffset
		filestream >> temp; // yoffset=##
		startPos = temp.find(L"=") + 1;
		ch.yOffset = (float)std::stoi(temp.substr(startPos, temp.size() - startPos));

		// get xadvance
		filestream >> temp; // xadvance=##
		startPos = temp.find(L"=") + 1;
		ch.xAdvance = (float)std::stoi(temp.substr(startPos, temp.size() - startPos));

		// dummy data
		filestream >> temp >> temp; // page=## chnl=##
	}

	// kerning generation
	filestream >> temp >> temp;	// kerning count=##
	startPos = temp.find(L"=") + 1;
	m_KerningCount = std::stoi(temp.substr(startPos, temp.size() - startPos));

	// alloc data
	m_KerningList = new FontKerning[m_KerningCount];

	for (size_t k = 0; k < m_KerningCount; ++k)
	{
		FontKerning & ke = m_KerningList[k];

		// get the first character
		filestream >> temp >> temp;	// kerning first=##
		startPos = temp.find(L"=") + 1;
		ke.FirstId = std::stoi(temp.substr(startPos, temp.size() - startPos));

		// get the second character
		filestream >> temp;	// second=##
		startPos = temp.find(L"=") + 1;
		ke.SecondId = std::stoi(temp.substr(startPos, temp.size() - startPos));

		// get amount
		filestream >> temp;	// amount=##
		startPos = temp.find(L"=") + 1;
		int t = (float)std::stoi(temp.substr(startPos, temp.size() - startPos));
		ke.Amount = (float)t;
	}

	// Font is loaded
	m_IsLoaded = true;
}

DX12Font::~DX12Font()
{
	// cleanup resources
	delete[] m_CharList;
	delete[] m_KerningList;

	// delete dx12 resources
	delete m_FontTexture;
}

float DX12Font::GetKerning(wchar_t i_First, wchar_t i_Second)
{
	return 0.0f;
}

DX12Font::FontChar * DX12Font::GetChar(wchar_t i_Char)
{
	return nullptr;
}

bool DX12Font::IsLoaded() const
{
	return m_IsLoaded;
}
