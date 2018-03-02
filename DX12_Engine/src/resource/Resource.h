// A resource is a descriptor, he describe a resource and is able from this resource to create and generate data for other objects (rendering for example)
// this also give the ability to store already loaded resources and do not reload them again (managed by the resource manager)
// a resource contains the code to load from disk data, convert them and push them if needed to a DX12Resource (GPU oriented resources)
// To do : reference counting and deletion if needed

#pragma once

#include <basetsd.h>
#include <string>

class Resource
{
public:
	// resource information
	UINT64					GetId() const;
	const std::string &		GetName() const;
	const std::string &		GetFilepath() const;
	bool					IsValid() const;	// the resource have no issues during loading (can be CPU or GPU) Warning : can be valid but not loaded already
	bool					IsLoaded() const;	// the resource is loaded onto the GPU and can be used

	// friend class
	friend class ResourceManager;
protected:
	std::string			m_Name;	// name of the resource (can be specific, this is used for editor and gameplay programmers purpose)
	std::string			m_Filepath;	// path of the resource (to the file that the resource come from, a file can contains more than one resource)

	// called from child to notify the end of the CPU loading
	void				NotifyFinishLoad();
	virtual void		Unload();	// this destroy CPU and GPU resource (calling release)
	virtual void		Release();	// this release the resource CPU side

	// path information helper
	std::string		RemovePath(const std::string & i_Path) const;	// this remove path and retreive the resource file
	std::string		ExtractFileName(const std::string & i_Path) const;
	std::string		ExtractFilePath(const std::string & i_Path) const;

	Resource();
	virtual ~Resource();

private:
	// load resource
	virtual void		LoadFromFile(const std::string & i_Filepath) = 0;
	virtual void		LoadFromData(const void * i_Data) = 0;

	// callbacks
	virtual void		FinishLoading();	// callback when the resource have finished loaded

	// information
	const UINT64		m_Id;
	bool				m_IsLoaded;
	bool				m_IsReleased;
};