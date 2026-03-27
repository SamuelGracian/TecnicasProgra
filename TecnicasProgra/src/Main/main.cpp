#include "Graphics/Dx11/DX11GraphicsAPI.h"
#include <fstream>
#include <string>
#include <sstream> 
#include <chrono> 
#include "mathfu/matrix.h"
#include "mathfu/constants.h"
#include <cstdint>

struct VERTEX
{               
    float x, y, z, w;
    float Color[4] = { 1,12,0,1 };
};

//struct ConstantBufferData
//{
//    float a;
//    float b;
//    float c;
//    float d;
//};

struct MoveVertex 
{
    float  cosValue, amplitude, c, d;
};

struct CameraMatrices 
{
    mathfu::Matrix<float, 4, 4> View;
    mathfu::Matrix<float, 4, 4> Perspective;
};

struct SimpleVertex
{
    mathfu::Vector<float,4> Pos;
    mathfu::Vector<float,2> Tex;
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


    mathfu::Vector<float,3> Eye(6.0f, 3.0f, 0.0f);
    mathfu::Vector<float,3> At(0.0f, 0.0f, 0.0f);
    mathfu::Vector<float,3> Up(0.0f, 1.0f, 0.0f);

    mathfu::Matrix<float, 4, 4> View = mathfu::Matrix<float, 4, 4>::LookAt(At, Eye, Up);
    mathfu::Matrix<float, 4, 4> Perspective = mathfu::Matrix<float, 4, 4>::Perspective((mathfu::kPi / 4), ((float)width / height), 0.01f, 100.0f, -1.0f);


    std::shared_ptr<DisplaySurface> window = std::make_shared<DisplaySurface>();
    window->init(width, height, L"Tecnicas Progra");

    std::shared_ptr<GRAPI> graphics = std::make_shared<DX11GraphicsAPI>();
    graphics->Init(window);

    std::shared_ptr<SwapChain> P_swapChain = graphics->CreateSwapChain(window, GAPI_FORMAT::FORMAT_R8G8B8A8_UNORM);

    std::shared_ptr<VertexShader> p_vertexShader = graphics->CreateVertexShader(ReadFileToString(L"Shaders/Shaders.fxh"), "VShader", defines);
    std::shared_ptr<PixelShader> p_pixelShader = graphics->CreatePixelShader(ReadFileToString(L"Shaders/Shaders.fxh"), "PShader", defines);

    auto RTView = P_swapChain->GetRenderTargetView();

    std::shared_ptr<DepthStencilView> depthStencilView = graphics->CreateDepthStencil(window->GetClientWidth(), window->GetClientHeight(), GAPI_FORMAT::FORMAT_D24_UNORM_S8_UINT);

    float amplitude = 0.5f; 
    MoveVertex moveData = { 1.0f, amplitude, 1.0f, 1.0f };
    CameraMatrices cameraData;
    cameraData.View = View;
    cameraData.Perspective = Perspective;

    std::shared_ptr<ConstantBuffer> constantBuffer = graphics->CreateConstantBuffer(sizeof(CameraMatrices), 0, &cameraData);

   /* VERTEX TriangleVertices[] =
    {
        {0.0f, 0.5f, 0.0f, 1.0f,  {1.0f, 0.0f, 0.0f, 1.0f} },
        {0.45f, -0.5f, 0.0f, 1.0f, {0.0f, 1.0f, 0.0f, 1.0f}},
        {-0.45f, -0.5f, 0.0f, 1.0f, {0.0f, 0.0f, 1.0f, 1.0f} }
    };*/

    SimpleVertex cubeVertices[] =
    {
        { mathfu::Vector<float,4>(-1.0f, 1.0f, -1.0f,1.0f), mathfu::Vector<float,2>(1.0f, 0.0f) },
        { mathfu::Vector<float,4>(1.0f, 1.0f, -1.0f,1.0f), mathfu::Vector<float,2>(0.0f, 0.0f) },
        { mathfu::Vector<float,4>(1.0f, 1.0f, 1.0f,1.0f), mathfu::Vector<float,2>(0.0f, 1.0f) },
        { mathfu::Vector<float,4>(-1.0f, 1.0f, 1.0f,1.0f), mathfu::Vector<float,2>(1.0f, 1.0f) },

        { mathfu::Vector<float,4>(-1.0f, -1.0f, -1.0f,1.0f), mathfu::Vector<float,2>(0.0f, 0.0f) },
        { mathfu::Vector<float,4>(1.0f, -1.0f, -1.0f,1.0f), mathfu::Vector<float,2>(1.0f, 0.0f) },
        { mathfu::Vector<float,4>(1.0f, -1.0f, 1.0f,1.0f), mathfu::Vector<float,2>(1.0f, 1.0f) },
        { mathfu::Vector<float,4>(-1.0f, -1.0f, 1.0f,1.0f), mathfu::Vector<float,2>(0.0f, 1.0f) },

        { mathfu::Vector<float,4>(-1.0f, -1.0f, 1.0f,1.0f), mathfu::Vector<float,2>(0.0f, 1.0f) },
        { mathfu::Vector<float,4>(-1.0f, -1.0f, -1.0f,1.0f), mathfu::Vector<float,2>(1.0f, 1.0f) },
        { mathfu::Vector<float,4>(-1.0f, 1.0f, -1.0f,1.0f), mathfu::Vector<float,2>(1.0f, 0.0f) },
        { mathfu::Vector<float,4>(-1.0f, 1.0f, 1.0f,1.0f), mathfu::Vector<float,2>(0.0f, 0.0f) },

        { mathfu::Vector<float,4>(1.0f, -1.0f, 1.0f,1.0f), mathfu::Vector<float,2>(1.0f, 1.0f) },
        { mathfu::Vector<float,4>(1.0f, -1.0f, -1.0f,1.0f), mathfu::Vector<float,2>(0.0f, 1.0f) },
        { mathfu::Vector<float,4>(1.0f, 1.0f, -1.0f,1.0f), mathfu::Vector<float,2>(0.0f, 0.0f) },
        { mathfu::Vector<float,4>(1.0f, 1.0f, 1.0f,1.0f), mathfu::Vector<float,2>(1.0f, 0.0f) },

        { mathfu::Vector<float,4>(-1.0f, -1.0f, -1.0f,1.0f), mathfu::Vector<float,2>(0.0f, 1.0f) },
        { mathfu::Vector<float,4>(1.0f, -1.0f, -1.0f,1.0f), mathfu::Vector<float,2>(1.0f, 1.0f) },
        { mathfu::Vector<float,4>(1.0f, 1.0f, -1.0f,1.0f), mathfu::Vector<float,2>(1.0f, 0.0f) },
        { mathfu::Vector<float,4>(-1.0f, 1.0f, -1.0f,1.0f), mathfu::Vector<float,2>(0.0f, 0.0f) },

        { mathfu::Vector<float,4>(-1.0f, -1.0f, 1.0f,1.0f), mathfu::Vector<float,2>(1.0f, 1.0f) },
        { mathfu::Vector<float,4>(1.0f, -1.0f, 1.0f,1.0f), mathfu::Vector<float,2>(0.0f, 1.0f) },
        { mathfu::Vector<float,4>(1.0f, 1.0f, 1.0f,1.0f), mathfu::Vector<float,2>(0.0f, 0.0f) },
        { mathfu::Vector<float,4>(-1.0f, 1.0f, 1.0f,1.0f), mathfu::Vector<float,2>(1.0f, 0.0f) },
    };

    std::shared_ptr<VertexBuffer> p_vertexBuffer = graphics->CreateVertexBuffer(sizeof(SimpleVertex)*24 , cubeVertices);

    ///Index buffer 

    std::uint16_t cubeIndex[] =
    {
        3,1,0,
        2,1,3,

        6,4,5,
        7,4,6,

        11,9,8,
        10,9,11,

        14,12,13,
        15,12,14,

        19,17,16,
        18,17,19,

        22,20,21,
        23,20,22
    };

    std::shared_ptr<IndexBuffer> p_indexBuffer = graphics->CreateIndexBuffer(sizeof(std::uint16_t) * 36, cubeIndex, 36);

    std::shared_ptr<Topology> p_topology = graphics->CreateTopology(Topology::Type::TriangleList);

    float clearColor[4] = { 0.0f, 0.5f, 0.8f, 1.0f };
    uint8_t Flag = DepthStencilView::ClearFlags::Depth | DepthStencilView::ClearFlags::Stencil;

    float time = 0.0f; 
    auto previous = std::chrono::high_resolution_clock::now();

    bool isAppRunning = true;
    while (isAppRunning)
    {
        window->processMessages();

        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = now - previous;
        previous = now;

        float deltaTime = elapsed.count();
        time += deltaTime;

        moveData.cosValue = std::cos(time);
        moveData.amplitude = amplitude;
        
        graphics->UpdateConstantBuffer(constantBuffer, sizeof(CameraMatrices), &cameraData);
        
        graphics->ClearRenderTargetView(RTView, clearColor);
        graphics->ClearDepthStencilView(depthStencilView, static_cast<DepthStencilView::ClearFlags>(Flag), 1.0f, 0);
        graphics->SetRenderTargetView(RTView, depthStencilView);

        
        graphics->SetTopology(p_topology);

        graphics->SetVertexShader(p_vertexShader);
        graphics->SetPixelShader(p_pixelShader);

        graphics->SetVertexBuffer(p_vertexBuffer, sizeof(SimpleVertex)*24 , 0);
        graphics->SetIndexBuffer(p_indexBuffer);
        graphics->SetConstantBuffer(constantBuffer);
        
        graphics->Draw(36, 0);
        
        P_swapChain->Present(0, 0);
    }

    return 0;
}