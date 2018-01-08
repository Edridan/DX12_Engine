#pragma once

#include <d3d12.h>

class DX12Shader
{
public:
	// Enum
	enum EShaderType
	{
		ePixel		= 0,
		eVertex		= 1,
	};

	// DX12Shader
	DX12Shader(EShaderType i_Type, const wchar_t *i_Filename);
	~DX12Shader();

	// Get/Set
	const D3D12_SHADER_BYTECODE & GetByteCode() const;

private:
	// Information
	const EShaderType	m_ShaderType;
	
	// DX12
	D3D12_SHADER_BYTECODE		m_ShaderByteCode;
};