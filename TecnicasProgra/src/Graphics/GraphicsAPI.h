#pragma once
#include <memory>
#include <string>
#include <vector>

#include "SwapChain.h"
#include "ConstantBuffer.h"
#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Topology.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "DepthStencilView.h"
#include "ViewPort.h"
#include "GraphicGenerals.h"
#include "Display/Window.h"
#include "Graphics/RenderTargetView.h"
#include "Texture2d.h"
#include "SamplerState.h"


class GRAPI
{
public:
	GRAPI() :
		m_shaderModel (0) 
	{ }

	~GRAPI() = default;

	virtual bool Init(std::weak_ptr<DisplaySurface> handleWindow) = 0;

	//Clean up function
	virtual void CleanUpResources() = 0;

	//Swap Chain
	virtual std::shared_ptr<SwapChain> CreateSwapChain(std::weak_ptr<DisplaySurface> handleWindow,
		GAPI_FORMAT::K = GAPI_FORMAT::FORMAT_UNKNOWN) = 0;

	//const buffer
	virtual std::shared_ptr<ConstantBuffer> CreateConstantBuffer(const uint32_t bytewidth = 0,
		const uint32_t slot = 0,
		void* data = nullptr) = 0;

	//Index buffer
	virtual std::shared_ptr<IndexBuffer> CreateIndexBuffer(const uint32_t bytewidth = 0,
		void* data = nullptr,
		uint32_t indexcount = 0) = 0;

	//Vertex buffer
	virtual std::shared_ptr<VertexBuffer> CreateVertexBuffer(const uint32_t bytewidth = 0,
		const void* vertices = nullptr) = 0;

	//Set buffers
	virtual void SetConstantBuffer(std::weak_ptr<ConstantBuffer> buffer)=0;


	virtual void SetIndexBuffer(std::weak_ptr<IndexBuffer> buffer) = 0; 

	//Update buffers
	virtual void UpdateConstantBuffer(std::weak_ptr<ConstantBuffer> buffer, const uint32_t bytewidth, void* Data = nullptr) = 0;

	//Topology
	virtual std::shared_ptr<Topology> CreateTopology(Topology::Type type = Topology::Type::TriangleList) = 0;

	virtual void SetTopology(std::weak_ptr<Topology> topology) = 0;

	//Shaders
	virtual std::shared_ptr<VertexShader> CreateVertexShader(const std::string& shaderCode, const std::string& entrypoint, std::vector<std::string> Defines) = 0;

	virtual std::shared_ptr<PixelShader> CreatePixelShader(const std::string& shaderCode, const std::string& entrypoint, std::vector<std::string> Defines) = 0;

	virtual void SetVertexShader(std::weak_ptr<VertexShader> shader) = 0;

	virtual void SetPixelShader(std::weak_ptr<PixelShader> shader) = 0;

	virtual void SetVertexBuffer(std::weak_ptr<VertexBuffer> buffer, uint32_t stride, uint32_t offset = 0) = 0;

	virtual void Draw(uint32_t vertexCount, uint32_t startVertexLocation = 0) = 0;

	//Depth stencil 
	virtual std::shared_ptr<DepthStencilView> CreateDepthStencil(uint32_t width = 0, uint32_t height = 0, 
		const GAPI_FORMAT::K format = GAPI_FORMAT::FORMAT_UNKNOWN) = 0;


	virtual std::shared_ptr<ViewPort> CreateViewPort(float width, float height, float minDepth, 
		float maxDepth, float topLeftX, float topLeftY) = 0;

	//Render target view
	virtual void SetRenderTargetView(std::weak_ptr<RenderTargetView> renderTargetView, std::weak_ptr<DepthStencilView> depthStencilView = std::weak_ptr<DepthStencilView>()) = 0;

	virtual void ClearRenderTargetView(std::weak_ptr<RenderTargetView> renderTargetView, float color[4]) = 0;

	virtual void ClearDepthStencilView(std::weak_ptr<DepthStencilView>depthStencil, DepthStencilView::ClearFlags flag , float depth, uint32_t stencil) = 0;


	/// Textures 
	//Sampler state
	virtual  std::shared_ptr<SamplerState> CreateSamplerState() = 0;

	virtual void SetSampler(uint32_t slot, std::weak_ptr<SamplerState> sampler) = 0;

	//texture 2d

	virtual std::vector<uint8_t> LoadImage(const std::string& filepath, int32_t* width, int32_t* height, int32_t* channels) = 0;

	virtual void SetTexture2D(uint32_t slot, std::weak_ptr <Texture2D> texture) = 0; 

	virtual std::shared_ptr<Texture2D> CreateTexture2D(std::vector<uint8_t> imageData, int32_t width, int32_t height) = 0;

protected:
	uint32_t m_shaderModel;

};