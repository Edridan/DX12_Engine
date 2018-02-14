#include "dx12/DX12Shader.h"

#include <d3dcompiler.h>
#include <string> 
#include <string.h>

DX12Shader::DX12Shader(EShaderType i_Type, const wchar_t * i_Filename, const D3D_SHADER_MACRO * i_Defines)
	:m_ShaderType(i_Type)
	,m_IsLoaded(false)
{
	wcscpy_s(m_Name, 128, i_Filename);

	// Load and generate the shader
	ID3DBlob* shader; // d3d blob for holding vertex shader bytecode
	ID3DBlob* errorBuff; // a buffer holding the error data if any

	HRESULT hr;

	// Select the shader target depending the shader type
	char shaderTarget[7] = "*s_5_0";

	switch (i_Type)
	{
	case ePixel:
		shaderTarget[0] = 'p';
		break;
	case eVertex:
		shaderTarget[0] = 'v';
		break;
	}

	// Compile shader from file
	hr = D3DCompileFromFile(i_Filename,
		i_Defines,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		shaderTarget,
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&shader,
		&errorBuff);

	if (FAILED(hr))
	{
		// Draw debug in display and messagebox
		MessageBoxA(NULL, (char*)errorBuff->GetBufferPointer(),
			"Shader compilation error", MB_OK | MB_ICONERROR);

		return;
	}

	// fill out shader bytecode structure for shader
	m_ShaderByteCode = {};
	m_ShaderByteCode.BytecodeLength = shader->GetBufferSize();
	m_ShaderByteCode.pShaderBytecode = shader->GetBufferPointer();

	m_IsLoaded = true;
}

DX12Shader::DX12Shader(EShaderType i_Type, const ShaderCode & i_Code)
	:m_ShaderType(i_Type)
	, m_Name(L"GeneratedShader")
	, m_IsLoaded(false)
{
	// procedural name generation
#pragma warning(disable:4311 4302)
	wchar_t wbuffer[64];
	_itow_s((int)this, wbuffer, 64);
	wcscpy_s(&m_Name[16], 64 * sizeof(wchar_t), wbuffer);
#pragma warning(default:4311 4302)

	// Load and generate the shader
	ID3DBlob* shader; // d3d blob for holding vertex shader bytecode
	ID3DBlob* errorBuff; // a buffer holding the error data if any

	HRESULT hr;

	// Select the shader target depending the shader type
	char shaderTarget[7] = "*s_5_0";

	switch (i_Type)
	{
	case ePixel:
		shaderTarget[0] = 'p';
		break;
	case eVertex:
		shaderTarget[0] = 'v';
		break;
	}

	// Compile shader from file
	hr = D3DCompile(i_Code.Code.c_str(),
		i_Code.Code.size(),
		nullptr,
		i_Code.Defines,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		shaderTarget,
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&shader,
		&errorBuff);

	if (FAILED(hr))
	{
		// Draw debug in display and messagebox
		MessageBoxA(NULL, (char*)errorBuff->GetBufferPointer(),
			"Shader compilation error", MB_OK | MB_ICONERROR);

		return;
	}

	// fill out shader bytecode structure for shader
	m_ShaderByteCode = {};
	m_ShaderByteCode.BytecodeLength = shader->GetBufferSize();
	m_ShaderByteCode.pShaderBytecode = shader->GetBufferPointer();

	m_IsLoaded = true;
}

DX12Shader::~DX12Shader()
{
}

const D3D12_SHADER_BYTECODE & DX12Shader::GetByteCode() const
{
	return m_ShaderByteCode;
}

bool DX12Shader::IsLoaded() const
{
	return m_IsLoaded;
}

DX12Shader::EShaderType DX12Shader::GetType() const
{
	return m_ShaderType;
}
