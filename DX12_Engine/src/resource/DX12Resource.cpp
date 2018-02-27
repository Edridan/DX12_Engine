#include "DX12Resource.h"

#include "engine/Debug.h"

DX12Resource::DX12Resource()
	:Resource()
{
}

DX12Resource::~DX12Resource()
{
}

void DX12Resource::LoadFromFile(const std::string & i_Filepath)
{
	ASSERT_ERROR("Do not use this LoadFromFile to load a DX12Resource");
}

void DX12Resource::LoadFromData(const void * i_Data)
{
	ASSERT_ERROR("Do not use this LoadFromData to load a DX12Resource");

}