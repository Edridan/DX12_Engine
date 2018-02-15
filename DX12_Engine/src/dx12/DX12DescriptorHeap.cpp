#include "DX12DescriptorHeap.h"

#include "DX12RenderEngine.h"

DX12DescriptorHeap::DX12DescriptorHeap(const D3D12_DESCRIPTOR_HEAP_DESC & i_Desc, const wchar_t * i_Name)
	:m_Size(0)
	,m_Count(0)
	,m_Name(i_Name)
{
	ID3D12Device * device = DX12RenderEngine::GetInstance().GetDevice();
	// create the descriptor heap
	HRESULT hr = device->CreateDescriptorHeap(&i_Desc, IID_PPV_ARGS(&m_DescriptorHeap));
	
	if (FAILED(hr))
	{
		ASSERT_ERROR("Unable to create descriptor heap");
		return;
	}

	// retreive data
	m_Size = device->GetDescriptorHandleIncrementSize(i_Desc.Type);
	m_Count = i_Desc.NumDescriptors;

	m_DescriptorHeap->SetName(i_Name);
}

DX12DescriptorHeap::~DX12DescriptorHeap()
{
	// release the resource
	SAFE_RELEASE(m_DescriptorHeap);
}

ID3D12DescriptorHeap * DX12DescriptorHeap::GetDescriptorHeap() const
{
	return m_DescriptorHeap;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DX12DescriptorHeap::GetCPUDescriptorHandle(UINT i_Index) const
{
	ASSERT((int)i_Index < m_Count);
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart(), i_Index, m_Size);
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DX12DescriptorHeap::GetCPUDescriptorHandle() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_DescriptorHeap->GetCPUDescriptorHandleForHeapStart());
}

CD3DX12_GPU_DESCRIPTOR_HANDLE DX12DescriptorHeap::GetGPUDescriptorHandle() const
{
	return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart());
}

CD3DX12_GPU_DESCRIPTOR_HANDLE DX12DescriptorHeap::GetGPUDescriptorHandle(UINT i_Index) const
{
	return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_DescriptorHeap->GetGPUDescriptorHandleForHeapStart(), i_Index, m_Size);
}


int DX12DescriptorHeap::GetDescriptorSize() const
{
	return m_Size;
}

int DX12DescriptorHeap::GetDescriptorCount() const
{
	return m_Count;
}

const wchar_t * DX12DescriptorHeap::GetName() const
{
	return m_Name.c_str();
}
