#include "Graphics/Dx11/DX11GraphicsAPI.h"
#include "Graphics/GraphicsAPI.h"
#include "mathfu/constants.h"
#include "mathfu/matrix.h"
#include <filesystem>

#include <fstream>
#include <string>
#include <sstream> 
#include <chrono> 
#include <iostream>
#include <vector>
#include <cstdint>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

struct VERTEX
{               
    float x, y, z, w;
    float Color[4] = { 0,0,0,0 };
};

struct MoveVertex 
{
    float  cosValue, amplitude, c, d;
};

struct CameraMatrices 
{
    mathfu::Matrix<float, 4, 4> View;
    mathfu::Matrix<float, 4, 4> Perspective;
    mathfu::Matrix<float, 4, 4> worldMatrix;
    mathfu::Vector <float, 3> cameraPosition;
    float Shininess;
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

//Allign Quad
struct QuadVertex
{
    float x, y;
    float u, v;
};


int main()
{
    uint32_t width = 800;
    uint32_t height = 600;

    mathfu::Vector<float,3> Eye(100.0f, 100.0f, 200.0f); 
    mathfu::Vector<float,3> At(0.0f, 0.0f, 0.0f);
    mathfu::Vector<float,3> Up(0.0f, 1.0f, 0.0f);

    mathfu::Matrix<float, 4, 4> View = mathfu::Matrix<float, 4, 4>::LookAt(At, Eye, Up);
    mathfu::Matrix<float, 4, 4> Perspective = mathfu::Matrix<float, 4, 4>::Perspective((mathfu::kPi / 4), ((float)width / height), 0.01f, 1000.0f, -1.0f);

    // Init Window
    std::shared_ptr<DisplaySurface> window = std::make_shared<DisplaySurface>();
    window->init(width, height, L"Tecnicas Progra");

    // Init GRAPI
    std::shared_ptr<GRAPI> graphics = std::make_shared<DX11GraphicsAPI>();
    graphics->Init(window);

    //Swap chain
    std::shared_ptr<SwapChain> P_swapChain = graphics->CreateSwapChain(window, GAPI_FORMAT::FORMAT_R8G8B8A8_UNORM);

    //Shaders
    //std::shared_ptr<VertexShader> p_vertexShader = graphics->CreateVertexShader(ReadFileToString(L"RawData/Shaders/Shaders.fxh"), "PShader", defines);
    std::shared_ptr<PixelShader> p_pixelShader = graphics->CreatePixelShader(ReadFileToString(L"RawData/Shaders/Shaders.fxh"), "PShader", defines);
    ///New Shaders
    std::shared_ptr<PixelShader> p_pixelShader_1 = graphics->CreatePixelShader(ReadFileToString(L"RawData/Shaders/PixelShader.fxh"), "PShader", defines);
    std::shared_ptr<VertexShader> p_vertexShader_1 = graphics->CreateVertexShader(ReadFileToString(L"RawData/Shaders/VertexShader.fxh"), "VShader", defines);

    std::shared_ptr<VertexShader> p_quadVertexShader = graphics->CreateVertexShader(ReadFileToString(L"RawData/Shaders/QuadVertexShader.fxh"), "VShader", defines);


    // Render target view 
    auto RTView = P_swapChain->GetRenderTargetView();

    std::shared_ptr<DepthStencilView> depthStencilView = graphics->CreateDepthStencil(window->GetClientWidth(), window->GetClientHeight(), GAPI_FORMAT::FORMAT_D24_UNORM_S8_UINT);

    float amplitude = 0.5f; 
    MoveVertex moveData = { 1.0f, amplitude, 1.0f, 1.0f };
    CameraMatrices cameraData;
    cameraData.View = View;
    cameraData.Perspective = Perspective;
    cameraData.cameraPosition = Eye;
    cameraData.Shininess = 2;

    std::shared_ptr<ConstantBuffer> constantBuffer = graphics->CreateConstantBuffer(sizeof(CameraMatrices), 0, &cameraData);

    // ==========================================
    // MODEL LOAD WITH ASSIMP
    // ==========================================
    std::vector<SimpleVertex> modelVertices;
    std::vector<uint16_t> modelIndices;

    // drakefire_pistol_low
    // CubeFile
    if (!graphics->ImportModelAsset_Assimp("Models/drakefire_pistol_low.obj", modelVertices, modelIndices))
    {
        std::cout << "Failed on Load model" << std::endl;
        return -1;
    }

    std::shared_ptr<VertexBuffer> p_vertexBuffer = nullptr;
    std::shared_ptr<IndexBuffer> p_indexBuffer = nullptr;
    uint32_t indexCount = 0;

    if (!modelVertices.empty() && !modelIndices.empty())
    {
        p_vertexBuffer = graphics->CreateVertexBuffer(sizeof(SimpleVertex) * modelVertices.size(), modelVertices.data());
        p_indexBuffer = graphics->CreateIndexBuffer(sizeof(uint16_t) * modelIndices.size(), modelIndices.data(), modelIndices.size());
        indexCount = modelIndices.size();
    }
    else 
    {
        std::cout << "Failed to construct buffers" << std::endl;
        return -1;
    }

    //topology
    std::shared_ptr<Topology> p_topology = graphics->CreateTopology(Topology::Type::TriangleList);

    int32_t imgWidth = 0;
    int32_t imgHeight = 0;
    int32_t imgChannels = 0;
    
    //load texture
    std::vector<uint8_t> pixels = graphics->LoadImageFromFile("textures/rocks.jpg", &imgWidth, &imgHeight, &imgChannels);
    
    std::shared_ptr<Texture2D> myTexture = nullptr;
    if (!pixels.empty()) 
    {
        myTexture = graphics->CreateTexture2D(pixels, imgWidth, imgHeight);
    }
    else 
    {
        std::cout << "Error: Texture couldn't be loaded" << std::endl;
    }

    //normals
    std::shared_ptr<Texture2D> normalTexture = graphics->CreateTexture2DFromFile("Textures/base_normal.jpg");

    std::shared_ptr<SamplerState> mySampler = graphics->CreateSamplerState();

    //albedo 
    std::shared_ptr <Texture2D> albedoTexture = graphics->CreateTexture2DFromFile("Textures/drakefire_pistol_low_a.jpg");

    //specular texture
    std::shared_ptr<Texture2D> specularTexture = graphics->CreateTexture2DFromFile("Textures/base_metallic.jpg");

    float clearColor[4] = { 0.0f, 0.5f, 0.8f, 1.0f };
    uint8_t Flag = DepthStencilView::ClearFlags::Depth | DepthStencilView::ClearFlags::Stencil;

    float time = 0.0f; 
    auto previous = std::chrono::high_resolution_clock::now();

    ////
    //Rasterizer state
    std::shared_ptr<RasterizerState> Rasterizer_pass1 = nullptr;
    // Cull none, fill solid, true ( se ve el arma pero se atravieza )
    Rasterizer_pass1 = graphics->CreateRasterizerState(CULL_MODE::CULL_BACK, FILL_MODE::FILL_SOLID, TRUE);
    if (!Rasterizer_pass1)
    {
        std::cout << "Failed to create rasterizer state" << std::endl;
        return -1;
    }



    std::shared_ptr<RasterizerState> Rasterizer_pass2 = nullptr;
    Rasterizer_pass2 = graphics->CreateRasterizerState(CULL_MODE::CULL_NONE, FILL_MODE::FILL_SOLID, FALSE);
    if (!Rasterizer_pass2)
    {
        std::cout << "Failed to create rasterizer state" << std::endl;
        return -1;
    }



    /////////////
    //second render target
    auto SecondRTV = graphics->CreateRenderTargetView(window->GetClientWidth(), window->GetClientHeight(), GAPI_FORMAT::FORMAT_R8G8B8A8_UNORM);
    std::vector<std::weak_ptr<RenderTargetView>> rtvs;
    rtvs.push_back(RTView);  
    rtvs.push_back(SecondRTV); 

   // G-buffer RTs
    auto NormalRTV = graphics->CreateRenderTargetView(window->GetClientWidth(), window->GetClientHeight(), GAPI_FORMAT::FORMAT_R8G8B8A8_UNORM);
    auto ColorRTV = graphics->CreateRenderTargetView(window->GetClientWidth(), window->GetClientHeight(), GAPI_FORMAT::FORMAT_R8G8B8A8_UNORM);
    auto SpecularRTV = graphics->CreateRenderTargetView(window->GetClientWidth(), window->GetClientHeight(), GAPI_FORMAT::FORMAT_R8G8B8A8_UNORM);

    std::vector<std::weak_ptr<RenderTargetView>> gbufferRTVs;
    gbufferRTVs.push_back(NormalRTV);
    gbufferRTVs.push_back(ColorRTV);
    gbufferRTVs.push_back(SpecularRTV);

    // quad
    QuadVertex quadVertices[] =
    {
        { -1.0f,  1.0f, 0.0f, 0.0f },
        {  1.0f,  1.0f, 1.0f, 0.0f },
        {  1.0f, -1.0f, 1.0f, 1.0f },
        { -1.0f, -1.0f, 0.0f, 1.0f },
    };

    uint16_t quadIndices[] =
    {
        0, 1, 2,
        0, 2, 3
    };

    std::shared_ptr<VertexBuffer> quadVB = graphics->CreateVertexBuffer(sizeof(quadVertices), quadVertices);
    std::shared_ptr<IndexBuffer>  quadIB = graphics->CreateIndexBuffer(sizeof(quadIndices), quadIndices, 6);
    std::shared_ptr<Topology>     quadTopology = graphics->CreateTopology(Topology::Type::TriangleList);

    bool isAppRunning = true;
    while (isAppRunning)
    {
        window->processMessages();

        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> elapsed = now - previous;
        previous = now;

        float deltaTime = elapsed.count();
        time += deltaTime;

        cameraData.worldMatrix = mathfu::Matrix<float, 4, 4>::FromScaleVector(mathfu::Vector<float, 3>(50, 50, 50));
        cameraData.worldMatrix = cameraData.worldMatrix * mathfu::Matrix<float, 4>::FromRotationMatrix(mathfu::Matrix<float, 4, 4>::RotationX(time * .5));

        moveData.cosValue = std::cos(time);
        moveData.amplitude = amplitude;

        graphics->UpdateConstantBuffer(constantBuffer, sizeof(CameraMatrices), &cameraData);
        
         

 // PASS 1: model
            graphics->SetRenderTargetViews(gbufferRTVs, depthStencilView);

            graphics->ClearRenderTargetView(NormalRTV, clearColor);
            graphics->ClearRenderTargetView(ColorRTV, clearColor);
            graphics->ClearRenderTargetView(SpecularRTV, clearColor);
            graphics->ClearDepthStencilView(depthStencilView, static_cast<DepthStencilView::ClearFlags>(Flag), 1.0f, 0);

            graphics->SetVertexShader(p_vertexShader_1);
            graphics->SetPixelShader(p_pixelShader_1);

            graphics->SetTopology(p_topology);
            graphics->SetVertexBuffer(p_vertexBuffer, sizeof(SimpleVertex), 0);
            graphics->SetIndexBuffer(p_indexBuffer);
            graphics->SetConstantBuffer(constantBuffer);

            graphics->SetSampler(0, mySampler);

            
            if (normalTexture)
            {
                graphics->SetTexture2D(2, normalTexture);

            }

            if (albedoTexture)
            {
                graphics->SetTexture2D(3, albedoTexture);

            }

            if (specularTexture)
            {
                graphics->SetTexture2D(4, specularTexture);
            }
            graphics->SetRasterizerState(Rasterizer_pass1);

            graphics->Draw(indexCount, 0);

            // PASS 2: quad -> backbuffer
            graphics->SetRenderTargetView(RTView, depthStencilView);
            graphics->ClearRenderTargetView(RTView, clearColor);
            graphics->ClearDepthStencilView(depthStencilView, static_cast<DepthStencilView::ClearFlags>(Flag), 1.0f, 0);

            graphics->SetVertexShader(p_quadVertexShader);
            graphics->SetPixelShader(p_pixelShader);

            graphics->SetTopology(quadTopology);
            graphics->SetVertexBuffer(quadVB, sizeof(QuadVertex), 0);
            graphics->SetIndexBuffer(quadIB);

            // bindings del G-buffer
            graphics->SetTexture2D(0, NormalRTV);
            graphics->SetTexture2D(1, ColorRTV);
            graphics->SetTexture2D(2, SpecularRTV);

            graphics->SetRasterizerState(Rasterizer_pass2);
            graphics->Draw(6, 0);

        P_swapChain->Present(0, 0);
    }

    return 0;
}
