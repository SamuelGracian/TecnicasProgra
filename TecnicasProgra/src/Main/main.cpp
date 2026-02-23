#include "Graphics/Dx11/DX11GraphicsAPI.h"
#include <fstream>
#include <string>
#include <sstream> 


        
struct VERTEX
{               
    float x, y, z;
    float Color[4] = { 1,12,0,1 };
};
        
std::vector<std::string> defines{ "#define TEST" };


std::string ReadFileToString(const std::wstring& filePath)
{
    std::ifstream file(filePath, std::ios::in | std::ios::binary);

    if (!file.is_open())
    {
        MessageBoxW(nullptr, L"Unable to open file", L"File Read Error", MB_OK);
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

int main()
{
    uint32_t width = 800;

    uint32_t height = 600;

  std::shared_ptr<DisplaySurface> window = std::make_shared<DisplaySurface>();
  window->init(width,height, L"Tecnicas Progra");

  std::shared_ptr<GRAPI> graphics = std::make_shared<DX11GraphicsAPI>();
  graphics->Init(window);

  //Swap chain
  std::shared_ptr<SwapChain> P_swapChain = graphics->CreateSwapChain(window, width,height, GAPI_FORMAT::FORMAT_R8G8B8A8_UNORM);

  //vertex shader
  std::shared_ptr<VertexShader> p_vertexShader = graphics->CreateVertexShader(ReadFileToString( L"Shaders.shaders"), "VS", defines);

  graphics->SetVertexShader(p_vertexShader);

  auto RTView = P_swapChain->GetRenderTargetView();

  graphics->SetRenderTargetView(RTView);

  VERTEX TriangleVertices[] =
  {
      {0.0f, 0.5f, 0.0f,  {1.0f, 0.0f, 0.0f, 1.0f} },
      {0.45f, -0.5, 0.0f, {0.0f, 1.0f, 0.0f, 1.0f}},
      {-0.45f, -0.5f, 0.0f, {0.0f, 0.0f, 1.0f, 1.0f} }
  };

  bool isAppRunning = true;
  while (isAppRunning)
  {
    window->processMessages();
    graphics->ClearSwapChain(P_swapChain);
    P_swapChain->Present(0,0);
  }

  return 1;
}