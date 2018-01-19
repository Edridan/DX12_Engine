#pragma once

#include <string>
#include "d3dx12.h"
#include "DX12Utils.h"

// class predef
class DX12Texture;

class DX12Font
{
public:
	// structure definition
	struct FontChar
	{
		// the unicode id
		int Id;

		// these need to be converted to texture coordinates 
		// (where 0.0 is 0 and 1.0 is textureWidth of the font)
		float U; // u texture coordinate
		float V; // v texture coordinate
		float tWidth; // width of character on texture
		float tHeight; // height of character on texture

		float Width; // width of character in screen coords
		float Height; // height of character in screen coords

					  // these need to be normalized based on size of font
		float xOffset; // offset from current cursor pos to left side of character
		float yOffset; // offset from top of line to top of character
		float xAdvance; // how far to move to right for next character
	};

	struct FontKerning
	{
		int FirstId;	// first character
		int SecondId;	// second character
		float Amount;	// The ammount to add/substract to second characters x
	};

	// DX12Font definition
	DX12Font(const wchar_t * i_FontDefinition);
	~DX12Font();

	// get information
	float		GetKerning(wchar_t i_First, wchar_t i_Second);
	FontChar *	GetChar(wchar_t i_Char);

	bool		IsLoaded() const;

private:
	// font information
	std::wstring		m_Name;
	std::wstring		m_FontImage;
	int					m_Size;
	float				m_LineHeight;
	float				m_BaseHeight;

	UINT				m_NumCharacters;
	FontChar *			m_CharList;

	size_t				m_KerningCount;
	FontKerning *		m_KerningList;

	Padding				m_Padding;
	IntVec2				m_TextureSize;

	DX12Texture *		m_FontTexture;

	bool				m_IsLoaded;		// succefully loaded
};