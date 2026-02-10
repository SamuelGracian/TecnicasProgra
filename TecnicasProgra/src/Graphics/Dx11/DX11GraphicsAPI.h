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
  
  std::shared_ptr<SwapChain> CreateSwapChain(std::weak_ptr<DisplaySurface> handleWindow, uint32_t width, uint32_t height, GAPI_FORMAT::K format) override;

  void ClearSwapChain(std::weak_ptr<SwapChain>swapChain) override;
												
private:

  IDXGISwapChain* CreateSwapChain_internal(HWND hwnd, uint32_t width, uint32_t height, GAPI_FORMAT::K format);

  ID3D11RenderTargetView* CreateBackBufferRT_internal(IDXGISwapChain* swapChain);

 private:
  ID3D11Device* m_device = nullptr;
  ID3D11DeviceContext* m_immediateContext = nullptr;
};