#pragma once

#include <d3d12.h>
#include <string>

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
	DX12Shader(EShaderType i_Type, const wchar_t *i_Filename, const D3D_SHADER_MACRO * i_Defines = nullptr);
	DX12Shader(EShaderType i_Type, const std::wstring & i_Code, const D3D_SHADER_MACRO * i_Defines = nullptr);
	~DX12Shader();

	// Get/Set
	const D3D12_SHADER_BYTECODE &	GetByteCode() const;
	bool							IsLoaded() const;
	EShaderType						GetType() const;

private:
	// Information
	const EShaderType	m_ShaderType;
	bool				m_IsLoaded;	// if true : is loaded and compiled = no error
	wchar_t 			m_Name[128];

	// DX12
	D3D12_SHADER_BYTECODE		m_ShaderByteCode;
};