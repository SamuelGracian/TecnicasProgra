
#include "Graphics/Dx11/DX11GraphicsAPI.h"


int main()
{
  std::shared_ptr<DisplaySurface> window = std::make_shared<DisplaySurface>();
  window->init(800,600, L"Tecnicas Progra");

  std::shared_ptr<GRAPI> graphics = std::make_shared<DX11GraphicsAPI>();
  graphics->Init(window);

  bool isAppRunning = true;
  while (isAppRunning)
  {
    window->processMessages();
  }

  return 1;
}