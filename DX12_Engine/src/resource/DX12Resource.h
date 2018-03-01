// GPU resource management
// this manage GPU resource only as DX12MeshBuffer and DX12Textures

// GPU side resource only
// this is managed by DX12ResourceManager that will load and push data to the GPU
#pragma once

#include "Resource.h"
#include "dx12/d3dx12.h"
#include <string>

class DX12Resource
{
public:
	// delete resource everywhere
	virtual ~DX12Resource();

	friend class DX12ResourceManager;

	UINT64				GetId() const;
	const std::string & GetName() const;
	const std::string & GetFilepath() const;
	bool				IsValid() const;	// the resource is valid and ready to be used

protected:
	// called by childs
	DX12Resource();

	virtual void		Release();	// release resources loaded on GPU

	std::string			m_Name;	// name of the resource (can be specific, this is used for editor and gameplay programmers purpose)
	std::string			m_Filepath;	// path of the resource (to the file that the resource come from, a file can contains more than one resource)

private:
	// load resource
	virtual void		LoadFromData(const void * i_Data, ID3D12GraphicsCommandList * i_CommandList, ID3D12Device * i_Device) = 0;
	virtual void		PreloadData(const void * i_Data);	// preload needed data for recognition (as name setup...)

	// callbacks
	void				FinishLoading();	// callback when the resource have finished loaded
	virtual void		NotifyFinishLoading();	// this is called in the childs and overriden if some resources need to be cleaned on the GPU
	// information
	const UINT64		m_Id;

	// information
	bool				m_IsLoaded;
};