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
	void CleanUpResources();

	virtual std::shared_ptr<SwapChain> CreateSwapChain(std::weak_ptr<DisplaySurface> handleWindow,uint32_t width = 0, uint32_t height = 0,
		GAPI_FORMAT::K = GAPI_FORMAT::FORMAT_UNKNOWN) = 0;

	virtual void ClearSwapChain(std::weak_ptr<SwapChain> swapChain) = 0;

	std::shared_ptr<ConstantBuffer> CreateConstantBuffer(const uint32_t bytewidth = 0,
		const uint32_t slot = 0,
		void* data = nullptr);

	std::shared_ptr<IndexBuffer> CreateIndexBuffer(const uint32_t bytewidth = 0,
		void* data = nullptr,
		uint32_t indexcount = 0);

	std::shared_ptr<VertexBuffer> CreateVertexBuffer(const uint32_t bytewidth = 0,
		const void* vertices = nullptr);

	//Set buffers
	void SetConstantBuffer(std::weak_ptr<ConstantBuffer> buffer);

	//Update buffers
	void UpdateConstantBuffer(std::weak_ptr<ConstantBuffer> buffer, const uint32_t bytewidth, void* Data = nullptr);

	std::shared_ptr<Topology> CreateTopology(Topology::Type type = Topology::Type::TriangleList);

	void SetTopology(std::weak_ptr<Topology> topology);

	std::shared_ptr<VertexShader> CreateVertexShader(const char* shader);

	std::shared_ptr<PixelShader> CreatePixelShader(const void* shaderBytecode, uint32_t bytecodeLenght);

	void SetVertexShader(std::weak_ptr<VertexShader> shader);

	std::shared_ptr<DepthStencilView> CreateDepthStencil(uint32_t width = 0, uint32_t height = 0, const GAPI_FORMAT::K format = GAPI_FORMAT::FORMAT_UNKNOWN);

	void CreateRenderTarget();

	void SetRenderTarget(const std::weak_ptr <DepthStencilView>& depthStencil);

	std::shared_ptr<ViewPort> CreateViewPort(float width, float height, float minDepth, float maxDepth, float topLeftX, float topLeftY);
};