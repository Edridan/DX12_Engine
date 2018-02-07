// helper for descriptor heap management
// this store a descriptor heap and manage size

#pragma once

#include "d3dx12.h"
#include <string>

class DX12DescriptorHeap
{
public:
	DX12DescriptorHeap(const D3D12_DESCRIPTOR_HEAP_DESC & i_Desc, const wchar_t * i_Name = L"Unnamed Descriptor Heap");
	~DX12DescriptorHeap();

	// dx12
	ID3D12DescriptorHeap *			GetDescriptorHeap() const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE	GetDescriptorHandle() const;
	CD3DX12_CPU_DESCRIPTOR_HANDLE	GetDescriptorHandle(UINT i_Index) const;

	// information
	int						GetDescriptorSize() const;
	int						GetDescriptorCount() const;
	const wchar_t *			GetName() const;

private:
	// dx12
	ID3D12DescriptorHeap *	m_DescriptorHeap;
	// internal
	int						m_Size;
	int						m_Count;
	const std::wstring		m_Name;
};