#include "Resource.h"

#include "engine/Utils.h"

UINT64 Resource::GetId() const
{
	return m_Id;
}

const std::string & Resource::GetName() const
{
	return m_Name;
}

const std::string & Resource::GetFilepath() const
{
	return m_Filepath;
}

bool Resource::IsValid() const
{
	return m_IsValid;
}

bool Resource::IsLoaded() const
{
	return m_IsLoaded;
}

void Resource::NotifyFinishLoad()
{
	m_IsLoaded = true;
}

inline std::string Resource::RemovePath(const std::string & i_Path) const
{
	int pos = i_Path.find_last_of('/');
	
	if (pos != std::string::npos)
	{
		return i_Path.substr(pos);
	}
	// unable to find the last "/" 
	return "";
}

inline std::string Resource::ExtractFileName(const std::string & i_Path) const
{
	std::string fileName = RemovePath(i_Path);
	int pos = fileName.find_last_of('.');

	if (pos != std::string::npos)
	{
		return fileName.substr(0, pos);
	}

	// unable to extract filename
	return "";
}

std::string Resource::ExtractFilePath(const std::string & i_Path) const
{
	int pos = i_Path.find_last_of('/');

	if (pos != std::string::npos)
	{
		return i_Path.substr(0, pos - 1);
	}
	// unable to find the last "/" 
	return "";
}

Resource::Resource()
	:m_Id((UINT64)this)
	,m_Filepath("Generated : " + String::Int64ToString(m_Id))	// will be changed if the resource is loaded from file
	,m_Name(m_Filepath)
{
}

void Resource::FinishLoading()
{
	m_IsLoaded = true;
}
