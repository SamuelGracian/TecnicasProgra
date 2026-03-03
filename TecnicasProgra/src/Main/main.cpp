#include "Graphics/Dx11/DX11GraphicsAPI.h"
#include <fstream>
#include <string>
#include <sstream> 


        
struct VERTEX
{               
    float x, y, z, w;
    float Color[4] = { 1,12,0,1 };
};

struct ConstantBufferData
{
    float a;
    float b;
    float c;
    float d;
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
    window->init(width, height, L"Tecnicas Progra");

    std::shared_ptr<GRAPI> graphics = std::make_shared<DX11GraphicsAPI>();
    graphics->Init(window);

    std::shared_ptr<SwapChain> P_swapChain = graphics->CreateSwapChain(window, width, height, GAPI_FORMAT::FORMAT_R8G8B8A8_UNORM);

    std::shared_ptr<VertexShader> p_vertexShader = graphics->CreateVertexShader(ReadFileToString(L"Shaders/Shaders.fxh"), "VShader", defines);
    
    std::shared_ptr<PixelShader> p_pixelShader = graphics->CreatePixelShader(ReadFileToString(L"Shaders/Shaders.fxh"), "PShader", defines);

    auto RTView = P_swapChain->GetRenderTargetView();

    std::shared_ptr<DepthStencilView> depthStencilView = graphics->CreateDepthStencil(width, height, GAPI_FORMAT::FORMAT_D24_UNORM_S8_UINT);

    ConstantBufferData cbData = { 1.0f, 2.0f, 3.0f, 4.0f };
    std::shared_ptr<ConstantBuffer> constantBuffer = graphics->CreateConstantBuffer(sizeof(ConstantBufferData), 0, &cbData);

    VERTEX TriangleVertices[] =
    {
        {0.0f, 0.5f, 0.0f, 1.0f,  {1.0f, 0.0f, 0.0f, 1.0f} },
        {0.45f, -0.5f, 0.0f, 1.0f, {0.0f, 1.0f, 0.0f, 1.0f}},
        {-0.45f, -0.5f, 0.0f, 1.0f, {0.0f, 0.0f, 1.0f, 1.0f} }
    };

    std::shared_ptr<VertexBuffer> p_vertexBuffer = graphics->CreateVertexBuffer(sizeof(TriangleVertices), TriangleVertices);

    std::shared_ptr<Topology> p_topology = graphics->CreateTopology(Topology::Type::TriangleList);

    float clearColor[4] = { 0.0f, 0.5f, 0.8f, 1.0f };

    float DeltaTime = 0.0f;
    bool isAppRunning = true;
    while (isAppRunning)
    {
        window->processMessages();
        
        DeltaTime += 0.016f;
        cbData.a = sin(DeltaTime);
        cbData.b = cos(DeltaTime);
        cbData.c = DeltaTime;
        cbData.d = 1.0f;
        
        graphics->UpdateConstantBuffer(constantBuffer, sizeof(ConstantBufferData), &cbData);
        
        graphics->ClearRenderTargetView(RTView, clearColor);
        
        graphics->SetRenderTargetView(RTView, depthStencilView);
        
        graphics->SetConstantBuffer(constantBuffer);
        
        graphics->SetTopology(p_topology);
        graphics->SetVertexShader(p_vertexShader);
        graphics->SetPixelShader(p_pixelShader);
        graphics->SetVertexBuffer(p_vertexBuffer, sizeof(VERTEX), 0);
        
        graphics->Draw(3, 0);
        
        P_swapChain->Present(0, 0);
    }

    return 0;
}