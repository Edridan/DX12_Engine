#pragma once

#include "Resource.h"
#include "dx12/DX12Utils.h"

// class predef
class DX12Texture;

class Texture : public Resource
{
public:
	// struct
	struct ImageDataDesc
	{
		int BitsPerPixel;
		int BytesPerRow;
		int ImageSize;
		int Width;
		int Height;
	};

	// retreive GPU data
	DX12Texture *		GetDX12Texture() const;

	friend class ResourceManager;
private:
	Texture();
	~Texture();

	// Inherited via Resource
	virtual void LoadFromFile(const std::string & i_Filepath) override;
	virtual void LoadFromData(const void * i_Data) override;

	// struct
	struct ImageDataDesc
	{
		int			BitsPerPixel;
		int			BytesPerRow;
		int			ImageSize;
		int			Width;
		int			Height;
		DXGI_FORMAT	Format;
	};

	// helpers
	int			LoadImageDataFromFile(BYTE ** o_Data, ImageDataDesc & o_ImageDesc, LPCWSTR i_Filename);

	// data
	BYTE *		m_Data;
	int			m_ImageSize;

	// dx12
	DX12Texture *		m_Texture;
};