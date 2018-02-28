#pragma once

// resource
class Mesh;
class Texture;
class Material;

#include <vector>
#include <map>

class ResourceManager
{
public:
	// resource loading (id a resource with the same filepath is loaded, this return the resource and not load the resource)
	Mesh *		GetMesh(const std::string & i_File);
	Material *	GetMaterial(const std::string & i_File);
	Texture *	GetTexture(const std::string & i_File);

	// To do : manage data generated resources (can be loaded with unique id)

	// get resource by name (this will not load resource)
	// warning : this will return nullptr if the resource is not already loaded
	Mesh *		GetMeshByName(const std::string & i_Name) const;
	Material *	GetMaterialByName(const std::string & i_Name) const;
	Texture *	GetTextureByName(const std::string & i_Name) const;
	// retreive multiple resource
	void		GetAllMeshByName(std::vector<Mesh*> o_Out, const std::string & i_Name) const;
	void		GetAllTexturesByName(std::vector<Texture*> o_Out, const std::string & i_Name) const;
	void		GetAllMaterialsByName(std::vector<Material*> o_Out, const std::string & i_Name) const;

	friend class Engine;
private:
	ResourceManager();
	~ResourceManager();

	// resources containers
	std::map<const std::string, Mesh *>		m_Meshes;	// mesh data
	std::map<const std::string, Texture*>	m_Textures;	// textures data
	std::map<const std::string, Material*>	m_Materials;

};