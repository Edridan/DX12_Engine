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
