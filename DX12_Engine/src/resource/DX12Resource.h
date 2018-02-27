// GPU resource management
// this manage GPU resource only as DX12MeshBuffer and DX12Textures

// GPU side resource only
// this is managed by DX12ResourceManager that will load and push data to the GPU
#pragma once

#include "Resource.h"
#include "dx12/d3dx12.h"
#include <string>

class DX12Resource : public Resource
{
public:
	// delete resource everywhere
	virtual ~DX12Resource();

	friend class DX12ResourceManager;

protected:
	// called by childs
	DX12Resource();

private:
	// load resource
	virtual void		LoadFromFile(const std::string & i_Filepath, ID3D12GraphicsCommandList * i_CommandList, ID3D12Device * i_Device) = 0;
	virtual void		LoadFromData(const void * i_Data, ID3D12GraphicsCommandList * i_CommandList, ID3D12Device * i_Device) = 0;

	// Inherited via Resource
	virtual void LoadFromFile(const std::string & i_Filepath) override;
	virtual void LoadFromData(const void * i_Data) override;
};