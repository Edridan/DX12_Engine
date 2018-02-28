#pragma once

#include "Resource.h"
#include "engine/Utils.h"

class DX12Material;

class Material : public Resource
{
public:
	struct MaterialData
	{
		// material data
		Color Ka = color::Pink, Kd = color::Pink, Ks = color::Pink, Ke = color::Pink;
		float Ns = 1000.f;
		std::string	ImagePath;
		// data info
		std::string	Name, Filepath;
	};

	// DX12
	DX12Material *		GetDX12Material(const std::string & i_Name) const;
	DX12Material *		GetDX12Material(size_t i_Index = 0) const;

	// friend class
	friend class ResourceManager;
private:
	Material();
	~Material();

	std::vector<DX12Material *>		m_Materials;	// a material (CPU side) can contains multiple materials

	// Inherited via Resource
	virtual void LoadFromFile(const std::string & i_Filepath) override;
	virtual void LoadFromData(const void * i_Data) override;
};