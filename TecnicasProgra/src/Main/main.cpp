
#include "Graphics/Dx11/DX11GraphicsAPI.h"


int main()
{
    uint32_t widht = 800;

    uint32_t height = 600;

  std::shared_ptr<DisplaySurface> window = std::make_shared<DisplaySurface>();
  window->init(widht,height, L"Tecnicas Progra");

  std::shared_ptr<GRAPI> graphics = std::make_shared<DX11GraphicsAPI>();
  graphics->Init(window);

  std::shared_ptr<SwapChain> SwapChain = graphics->CreateSwapChain(window, widht,height, GAPI_FORMAT::FORMAT_R8G8B8A8_UNORM);

  graphics->ClearSwapChain(SwapChain);

  bool isAppRunning = true;
  while (isAppRunning)
  {
    window->processMessages();
  }

  return 1;
}