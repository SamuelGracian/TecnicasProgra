#pragma once
#include <memory>

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


class GRAPI
{
public:
	GRAPI() = default;
	~GRAPI() = default;

	virtual bool Init(std::weak_ptr<DisplaySurface> handleWindow) = 0;

	//Clean up function
	virtual void CleanUpResources() = 0;

	virtual std::shared_ptr<SwapChain> CreateSwapChain(std::weak_ptr<DisplaySurface> handleWindow,uint32_t width = 0,
		uint32_t height = 0,
		GAPI_FORMAT::K = GAPI_FORMAT::FORMAT_UNKNOWN) = 0;

	virtual void ClearSwapChain(std::weak_ptr<SwapChain> swapChain) = 0;

	virtual std::shared_ptr<ConstantBuffer> CreateConstantBuffer(const uint32_t bytewidth = 0,
		const uint32_t slot = 0,
		void* data = nullptr) = 0;

	virtual std::shared_ptr<IndexBuffer> CreateIndexBuffer(const uint32_t bytewidth = 0,
		void* data = nullptr,
		uint32_t indexcount = 0) = 0;

	virtual std::shared_ptr<VertexBuffer> CreateVertexBuffer(const uint32_t bytewidth = 0,
		const void* vertices = nullptr) = 0;

	//Set buffers
	virtual void SetConstantBuffer(std::weak_ptr<ConstantBuffer> buffer)=0;

	//Update buffers
	virtual void UpdateConstantBuffer(std::weak_ptr<ConstantBuffer> buffer, const uint32_t bytewidth, void* Data = nullptr) = 0;

	virtual std::shared_ptr<Topology> CreateTopology(Topology::Type type = Topology::Type::TriangleList) = 0;

	virtual void SetTopology(std::weak_ptr<Topology> topology) = 0;

	virtual std::shared_ptr<VertexShader> CreateVertexShader(const char* shader) = 0;

	virtual std::shared_ptr<PixelShader> CreatePixelShader(const void* shaderBytecode, uint32_t bytecodeLenght) = 0;

	virtual void SetVertexShader(std::weak_ptr<VertexShader> shader) = 0;

	virtual std::shared_ptr<DepthStencilView> CreateDepthStencil(uint32_t width = 0, uint32_t height = 0, 
		const GAPI_FORMAT::K format = GAPI_FORMAT::FORMAT_UNKNOWN) = 0;

	virtual void CreateRenderTarget() = 0;

	virtual void SetRenderTarget(const std::weak_ptr <DepthStencilView>& depthStencil) = 0;

	virtual std::shared_ptr<ViewPort> CreateViewPort(float width, float height, float minDepth, 
		float maxDepth, float topLeftX, float topLeftY) = 0;

};