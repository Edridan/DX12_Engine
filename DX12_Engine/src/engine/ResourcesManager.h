// Manage load and unload resources

#pragma once

#include <map>
#include <string>

// dx12 class predef
class DX12Mesh;
class DX12Font;
class DX12Texture;

// define for error loading resource
#define		ERROR_ID		L"error_resource"

class ResourcesManager
{
public:
	ResourcesManager();
	~ResourcesManager();

	// load/unload objects
	DX12Mesh *		GetMesh(const wchar_t * i_Filepath);
	DX12Texture *	GetTexture(const wchar_t * i_Filepath);
	
	// cleanup resources
	void			CleanUpResources();

	// iterators
	std::map<const std::wstring, DX12Mesh*>::iterator		GetBeginMesh();
	std::map<const std::wstring, DX12Mesh*>::iterator		GetEndMesh();
	std::map<const std::wstring, DX12Texture*>::iterator	GetBeginTexture();
	std::map<const std::wstring, DX12Texture*>::iterator	GetEndTexture();

	// To do : release resources with count


	// friend class
	friend class Engine;
private:

	// internal engine call
	bool			LoadErrorTexture(const wchar_t * i_Filepath);

	template <class _Type>
	void			CleanResourcesMap(std::map<const std::wstring, _Type> & i_Map) const;

	// resources management
	std::map<const std::wstring, DX12Mesh*>		m_Meshes;
	std::map<const std::wstring, DX12Texture*>	m_Textures;

};

template<class _Type>
inline void ResourcesManager::CleanResourcesMap(std::map<const std::wstring, _Type> & i_Map) const
{
	std::map<std::wstring, _Type>::iterator itr = i_Map.begin();

	while (itr != i_Map.end())
	{
		// delete meshes
		delete (*itr).second;
		++itr;
	}

	i_Map.clear();
}
