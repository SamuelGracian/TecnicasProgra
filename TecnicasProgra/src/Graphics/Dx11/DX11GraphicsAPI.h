#pragma once
#include "Graphics/GraphicsAPI.h"
#include "Graphics/GraphicGenerals.h"
#include "Display/Window.h"

#include <memory>
#include <d3d11.h>
#include <dxgi1_2.h>
#include <dxgi.h>

class DX11GraphicsAPI : public GRAPI
{
public:
  DX11GraphicsAPI();

  bool Init(std::weak_ptr<DisplaySurface> handleWindow) override;

  void CleanUpResources() override;
  
  //Swap Chain
  std::shared_ptr<SwapChain> CreateSwapChain(std::weak_ptr<DisplaySurface> handleWindow, GAPI_FORMAT::K format) override;


	//Buffers
	
   std::shared_ptr<ConstantBuffer> CreateConstantBuffer(const uint32_t bytewidth = 0,
	  const uint32_t slot = 0,
	  void* data = nullptr) override;

  std::shared_ptr<IndexBuffer> CreateIndexBuffer(const uint32_t bytewidth = 0,
	  void* data = nullptr,
	  uint32_t indexcount = 0) override;

  std::shared_ptr<VertexBuffer> CreateVertexBuffer(const uint32_t bytewidth = 0,
	  const void* vertices = nullptr) override;

  void SetConstantBuffer(std::weak_ptr<ConstantBuffer> buffer)override;

  void SetIndexBuffer(std::weak_ptr<IndexBuffer> buffer)override;

  //Update buffers
 void UpdateConstantBuffer(std::weak_ptr<ConstantBuffer> buffer, const uint32_t bytewidth, void* Data = nullptr) override;

 std::shared_ptr<Topology> CreateTopology(Topology::Type type = Topology::Type::TriangleList)override;

 void SetTopology(std::weak_ptr<Topology> topology) override;

 std::shared_ptr<VertexShader> CreateVertexShader(const std::string& shaderCode, const std::string& entrypoint, std::vector<std::string> Defines) override;

 std::shared_ptr<PixelShader> CreatePixelShader(const std::string& shaderCode, const std::string& entrypoint, std::vector<std::string> Defines)override;

 void SetVertexShader(std::weak_ptr<VertexShader> shader)override;

 void SetPixelShader(std::weak_ptr<PixelShader> shader)override;

 void SetVertexBuffer(std::weak_ptr<VertexBuffer> buffer, uint32_t stride, uint32_t offset = 0)override;

 void Draw(uint32_t vertexCount, uint32_t startVertexLocation = 0)override;

 //depth stencil
  std::shared_ptr<DepthStencilView> CreateDepthStencil(uint32_t width = 0, uint32_t height = 0,
	  const GAPI_FORMAT::K format = GAPI_FORMAT::FORMAT_UNKNOWN)override;


  //view port
  std::shared_ptr<ViewPort> CreateViewPort(float width, float height, float minDepth,
	  float maxDepth, float topLeftX, float topLeftY)override;

  //render target view

  std::shared_ptr<RenderTargetView> CreateRenderTargetView(uint32_t width, uint32_t height, GAPI_FORMAT::K format) override;

  void SetRenderTargetView(std::weak_ptr<RenderTargetView> renderTargetView, std::weak_ptr<DepthStencilView> depthStencilView = std::weak_ptr<DepthStencilView>()) override;

  void SetRenderTargetViews(const std::vector<std::weak_ptr<RenderTargetView>>& renderTargetViews, std::weak_ptr<DepthStencilView> depthStencilView = std::weak_ptr<DepthStencilView>()) override;

  void ClearRenderTargetView(std::weak_ptr<RenderTargetView> renderTargetView, float color[4]);

  void ClearDepthStencilView(std::weak_ptr<DepthStencilView>depthStencil, DepthStencilView::ClearFlags flag , float depth, uint32_t stencil) override;

  /// textures

  //sampler state 
  std::shared_ptr<SamplerState> CreateSamplerState() override;

  void SetSampler(uint32_t slot, std::weak_ptr<SamplerState> sampler) override;

  // texture 
  std::vector<uint8_t> LoadImageFromFile(const std::string& filepath, int32_t* width, int32_t* height, int32_t* channels) override;
	
  void SetTexture2D (uint32_t slot, std::weak_ptr<Texture2D> texture) override;

  std::shared_ptr<Texture2D>  CreateTexture2D(const std::vector<uint8_t>& imageData, int32_t width, int32_t height) override;

  bool ImportModelAsset_Assimp(const std::string& filename, std::vector<SimpleVertex>& outVertices, std::vector<uint16_t>& outIndices) override;

  // Rasterizer State
  std::shared_ptr <RasterizerState> CreateRasterizerState(CULL_MODE::K cullMode, FILL_MODE::K fillMode ) override;

  void SetRasterizerState() override;

private:

	// Internal functions
  IDXGISwapChain* CreateSwapChain_internal(HWND hwnd, uint32_t width, uint32_t height, GAPI_FORMAT::K format);

  ID3D11RenderTargetView* CreateBackBufferRT_internal(IDXGISwapChain* swapChain);

  ID3DBlob* CompileShader_internal(const std::string& shaderCode, const std::string& entrypoint, 
	  std::vector<std::string> Defines, SHADER_TYPE::K shaderType);

  ID3D11Texture2D* CreateTexture2D_internal(uint32_t width, uint32_t height, const GAPI_FORMAT::K format, uint32_t bindFlags);

  /// normals
  std::shared_ptr<Texture2D> CreateTexture2DFromFile(const std::string& filepath) override;


 private:
  ID3D11Device* m_device = nullptr;
  ID3D11DeviceContext* m_immediateContext = nullptr;
};