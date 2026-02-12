#include "Graphics/GraphicsAPI.h"
#include "Graphics/GraphicGenerals.h"
#include "Display/Window.h"

#include <d3d11.h>
#include <dxgi1_2.h>
#include <dxgi.h>
// Aquí si puedes inlcuir cosas de DirectX11

class DX11GraphicsAPI : public GRAPI
{
public:
  DX11GraphicsAPI();

  bool Init(std::weak_ptr<DisplaySurface> handleWindow) override;
  
  //Swap Chain
  std::shared_ptr<SwapChain> CreateSwapChain(std::weak_ptr<DisplaySurface> handleWindow, uint32_t width, uint32_t height, GAPI_FORMAT::K format) override;

  void ClearSwapChain(std::weak_ptr<SwapChain>swapChain) override;

	//Buffers
	
   std::shared_ptr<ConstantBuffer> CreateConstantBuffer(const uint32_t bytewidth = 0,
	  const uint32_t slot = 0,
	  void* data = nullptr);

  std::shared_ptr<IndexBuffer> CreateIndexBuffer(const uint32_t bytewidth = 0,
	  void* data = nullptr,
	  uint32_t indexcount = 0) override;

  std::shared_ptr<VertexBuffer> CreateVertexBuffer(const uint32_t bytewidth = 0,
	  const void* vertices = nullptr) override;
private:

  IDXGISwapChain* CreateSwapChain_internal(HWND hwnd, uint32_t width, uint32_t height, GAPI_FORMAT::K format);

  ID3D11RenderTargetView* CreateBackBufferRT_internal(IDXGISwapChain* swapChain);

 private:
  ID3D11Device* m_device = nullptr;
  ID3D11DeviceContext* m_immediateContext = nullptr;
};