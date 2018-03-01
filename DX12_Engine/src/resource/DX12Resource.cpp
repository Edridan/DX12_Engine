#include "DX12Resource.h"

#include "engine/Debug.h"

DX12Resource::DX12Resource()
	:m_Id((UINT64)this)
{
}

void DX12Resource::Release()
{
	m_IsLoaded = false;
}

DX12Resource::~DX12Resource()
{
}

UINT64 DX12Resource::GetId() const
{
	return m_Id;
}

const std::string & DX12Resource::GetName() const
{
	return m_Name;
}

const std::string & DX12Resource::GetFilepath() const
{
	return m_Filepath;
}

bool DX12Resource::IsValid() const
{
	return m_IsLoaded;
}

void DX12Resource::FinishLoading()
{
	m_IsLoaded = true;
	NotifyEndFinish();
}

void DX12Resource::NotifyEndFinish()
{
	// To do : implement in some child resource that need clean resource on GPU
}
