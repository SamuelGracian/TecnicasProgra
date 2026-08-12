#pragma once 
#include <d3d11.h>

#include "Graphics/VertexShader.h"

class Dx11VertexShader : public VertexShader
{
public:
	Dx11VertexShader() :
		m_shader(nullptr), m_InputLayout(nullptr)
	{ }


	~Dx11VertexShader() = default;


	ID3D11VertexShader* m_shader;
	ID3D11InputLayout* m_InputLayout;
};