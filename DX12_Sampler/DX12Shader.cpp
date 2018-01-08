#include "DX12Shader.h"

#include <d3dcompiler.h>

DX12Shader::DX12Shader(EShaderType i_Type, const wchar_t * i_Filename)
	:m_ShaderType(i_Type)
{
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
		nullptr,
		nullptr,
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

}

DX12Shader::~DX12Shader()
{
}

const D3D12_SHADER_BYTECODE & DX12Shader::GetByteCode() const
{
	return m_ShaderByteCode;
}
