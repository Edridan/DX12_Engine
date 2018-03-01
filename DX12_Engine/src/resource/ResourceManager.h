#pragma once

// resource
class Resource;
// specific
class Mesh;
class Texture;
class Material;

#include <basetsd.h>	// types UINT64
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

	// retreive an already resource with id
	Mesh *			GetMeshById(UINT64 i_Id) const;
	Texture *		GetTextureById(UINT64 i_Id) const;
	Material *		GetMaterialById(UINT64 i_Id) const;

	// informations
	

	// research resource by id
	Resource *		GetResourceById(UINT64 i_Id) const;

	// release resource
	bool			ReleaseResource(const UINT64 i_Id);	// release resource by Id

	friend class Engine;
private:
	ResourceManager();
	~ResourceManager();

	// resource mapper
	std::map<const UINT64, Resource *>	m_AllResources;

	// file name reseacher
	std::map<const std::string, Mesh *>		m_Meshes;		// mesh data
	std::map<const std::string, Texture*>	m_Textures;		// textures data
	std::map<const std::string, Material*>	m_Materials;	// materials data

	// id researcher
	// this contains all generated or not generated resources (there can be more resources)
	std::map<const UINT64, Mesh*>		m_MeshesId;		
	std::map<const UINT64, Texture*>	m_TexturesId;
	std::map<const UINT64, Material*>	m_MaterialsId;

};