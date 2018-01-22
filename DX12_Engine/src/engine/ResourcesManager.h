// Manage load and unload resources

#pragma once

#include <map>
#include <string>

// dx12 class predef
class DX12Mesh;
class DX12Font;
class DX12Texture;

class ResourcesManager
{
public:
	ResourcesManager();
	~ResourcesManager();

	// load/unload objects
	DX12Mesh *		GetMesh(const wchar_t * i_Filepath);
	DX12Font *		GetFont(const wchar_t * i_Filepath);
	
	// cleanup resources
	void			CleanUpResources();

private:
	std::map<std::wstring, DX12Mesh*>		m_Meshes;
	std::map<std::wstring, DX12Font*>		m_Fonts;

};