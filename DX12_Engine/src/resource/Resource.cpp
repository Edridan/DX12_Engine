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
	return m_IsLoaded && !m_IsReleased;
}

bool Resource::IsLoaded() const
{
	return m_IsLoaded;
}

void Resource::NotifyFinishLoad()
{
	m_IsLoaded = true;
}

void Resource::Release()
{
	// the resource can be still GPU loaded
	m_IsReleased = true;
}

std::string Resource::RemovePath(const std::string & i_Path) const
{
	size_t pos = i_Path.find_last_of('/');
	
	if (pos != std::string::npos)
	{
		return i_Path.substr(pos + 1);
	}
	// unable to find the last "/" 
	return "";
}

std::string Resource::ExtractFileName(const std::string & i_Path) const
{
	std::string fileName = RemovePath(i_Path);
	size_t pos = fileName.find_last_of('.');

	if (pos != std::string::npos)
	{
		return fileName.substr(0, pos);
	}

	// unable to extract filename
	return "";
}

std::string Resource::ExtractFilePath(const std::string & i_Path) const
{
	size_t pos = i_Path.find_last_of('/');

	if (pos != std::string::npos)
	{
		return i_Path.substr(0, pos + 1);
	}
	// unable to find the last "/" 
	return "";
}

Resource::Resource()
	:m_Id((UINT64)this)
	,m_IsLoaded(false)
	,m_IsReleased(false)
{
	m_Filepath	= "Generated:" + String::UInt64ToString(m_Id);
	m_Name		= m_Filepath;
}

Resource::~Resource()
{
}

void Resource::FinishLoading()
{
	m_IsLoaded = true;
}
