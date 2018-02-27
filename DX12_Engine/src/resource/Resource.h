// A resource can be a CPU or GPU resource management

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
	friend class DX12ResourceManager;
protected:
	std::string			m_Name;	// name of the resource (can be specific, this is used for editor and gameplay programmers purpose)
	std::string			m_Filepath;	// path of the resource (to the file that the resource come from, a file can contains more than one resource)

	Resource();
	virtual ~Resource() = 0;

private:
	// load resource
	virtual void		LoadFromFile(const std::string & i_Filepath) = 0;
	virtual void		LoadFromData(const void * i_Data) = 0;

	// callbacks
	virtual void		FinishLoading();	// callback when the resource have finished loaded

	// information
	const UINT64		m_Id;
	
	bool				m_IsValid;
	bool				m_IsLoaded;
};