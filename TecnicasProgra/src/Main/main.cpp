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

struct GeneralConstBuffer 
{
    mathfu::Matrix<float, 4, 4> View;
    mathfu::Matrix<float, 4, 4> Perspective;
    mathfu::Matrix<float, 4, 4> worldMatrix;
    mathfu::Matrix< float, 4, 4> ShadowView;
    mathfu::Matrix<float, 4, 4> ShadowProjection;
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
    mathfu::Matrix<float, 4, 4> Perspective = mathfu::Matrix<float, 4, 4>::Perspective(mathfu::kPi / 4.0f, (float)width / (float)height, 0.01f, 1000.0f, -1.0f);


    ///Second Camera
    mathfu::Vector<float, 3> LightEye ( 100.0f, 100.0f, 200.0f);

    mathfu::Vector<float, 3> LightDirection = (At - LightEye).Normalized();

    mathfu::Matrix<float, 4, 4> ShadowView = mathfu::Matrix<float, 4, 4>::LookAt(At, LightEye, Up);

    mathfu::Matrix<float, 4, 4> ShadowProjection = mathfu::Matrix<float, 4, 4>::Ortho(-100,100.0, -100.0f, 100.0f,-1000.0f,1000.0f, -1.0f);


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


    std::shared_ptr<PixelShader> p_ShadowPixelShader = graphics->CreatePixelShader(ReadFileToString(L"RawData/Shaders/ShadowPixelShader.fxh"), "PShader", defines);
    std::shared_ptr<VertexShader> p_ShadowVertexshader = graphics->CreateVertexShader(ReadFileToString(L"RawData/Shaders/ShadowVertexShader.fxh"), "VShader", defines);


    // Render target view 
    auto RTView = P_swapChain->GetRenderTargetView();

    std::shared_ptr<DepthStencilView> depthStencilView = graphics->CreateDepthStencil(window->GetClientWidth(), window->GetClientHeight(), GAPI_FORMAT::FORMAT_D24_UNORM_S8_UINT);

    float amplitude = 0.5f; 
    MoveVertex moveData = { 1.0f, amplitude, 1.0f, 1.0f };
    GeneralConstBuffer cameraData;
    cameraData.View = View;
    cameraData.Perspective = Perspective;
    cameraData.cameraPosition = Eye;
    cameraData.Shininess = 2;
    cameraData.ShadowProjection = ShadowProjection;
    cameraData.ShadowView = ShadowView;

    std::shared_ptr<ConstantBuffer> constantBuffer = graphics->CreateConstantBuffer(sizeof(GeneralConstBuffer), 0, &cameraData);

    // ==========================================
    // MODEL LOAD WITH ASSIMP
    // ==========================================
    std::vector<SimpleVertex> PistolVertices;
    std::vector<uint16_t> modelIndices;

    // drakefire_pistol_low
    // CubeFile

    /// Pistol model
    if (!graphics->ImportModelAsset_Assimp("Models/drakefire_pistol_low.obj", PistolVertices, modelIndices))
    {
        std::cout << "Failed on Load model" << std::endl;
        return -1;
    }

    /// Cube model
    std::vector<SimpleVertex> CubeVertices;
    std::vector<uint16_t> CubeIndices;
    if (!graphics->ImportModelAsset_Assimp("Models/CubeFile.obj", CubeVertices, CubeIndices))
    {
        std::cout << "Failed on Load model" << std::endl;
        return -1;
    }
    // cubo usado como piso: ancho/profundo reducidos, muy delgado en Y
    mathfu::Matrix<float, 4, 4> cubeWorldMatrix = mathfu::Matrix<float, 4, 4>::FromScaleVector( mathfu::Vector<float, 3>(10.0f, 0.25f, 10.0f)) * mathfu::Matrix<float, 4, 4>::FromTranslationVector(mathfu::Vector<float, 3>(0.0f, -0.25f, 0.0f));

    std::shared_ptr<VertexBuffer> p_pistolVertexBuffer = nullptr;
    std::shared_ptr<IndexBuffer> p_pistolIndexBuffer = nullptr;

    std::shared_ptr<VertexBuffer> p_cubeVertexBuffer = nullptr;
    std::shared_ptr<IndexBuffer> p_cubeIndexBuffer = nullptr;
    uint32_t pistolIndexCount = 0;
    uint32_t cubeIndexCount = 0;

    if (!PistolVertices.empty() && !modelIndices.empty())
    {
        p_pistolVertexBuffer = graphics->CreateVertexBuffer(sizeof(SimpleVertex) * PistolVertices.size(), PistolVertices.data());
        p_pistolIndexBuffer = graphics->CreateIndexBuffer(sizeof(uint16_t) * modelIndices.size(), modelIndices.data(), modelIndices.size());

        p_cubeVertexBuffer = graphics->CreateVertexBuffer(sizeof(SimpleVertex) * CubeVertices.size(), CubeVertices.data());
        p_cubeIndexBuffer = graphics->CreateIndexBuffer(sizeof(uint16_t) * CubeIndices.size(), CubeIndices.data(), CubeIndices.size());

        pistolIndexCount = modelIndices.size() ;
        cubeIndexCount = CubeIndices.size();
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
    
    // Cube texture
    std::shared_ptr<Texture2D> cubeTexture = graphics->CreateTexture2DFromFile("Textures/rocks.jpg");

    //normals
    std::shared_ptr<Texture2D> normalTexture = graphics->CreateTexture2DFromFile("Textures/base_normal.jpg");

    //Sampler state
    std::shared_ptr<SamplerState> mySampler = graphics->CreateSamplerState();

    //albedo 
    std::shared_ptr <Texture2D> albedoTexture = graphics->CreateTexture2DFromFile("Textures/drakefire_pistol_low_a.jpg");

    //specular texture
    std::shared_ptr<Texture2D> specularTexture = graphics->CreateTexture2DFromFile("Textures/base_metallic.jpg");

    float clearColor[4] = { 0.0f, 0.5f, 0.8f, 1.0f };
    float blackColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
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

    std::shared_ptr<RasterizerState> shadowRasterizer = nullptr;
    shadowRasterizer = graphics->CreateRasterizerState(CULL_MODE::CULL_NONE, FILL_MODE::FILL_SOLID, TRUE);
    if (!shadowRasterizer)
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

    //Shadow render target,  usar render target para profundidad 
    auto ShadowRenderTarget = graphics->CreateRenderTargetView(window->GetClientWidth(), window->GetClientHeight(), GAPI_FORMAT::FORMAT_R32_FLOAT);
    std::vector<std::weak_ptr<RenderTargetView>> SahdowRTVS;
    SahdowRTVS.push_back(ShadowRenderTarget);

    int shadowWidth = 800;
    int shadowHeight = 600;
    auto shadowDepthView = graphics->CreateShadowMap(shadowWidth, shadowHeight);
    if (!shadowDepthView)
    {
        std::cout << "Failed to create shadow map" << std::endl;
    }

    /// Render loop
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

        graphics->UpdateConstantBuffer(constantBuffer, sizeof(GeneralConstBuffer), &cameraData);
        

 //// pass 0 shadow pre process 

        if (shadowDepthView)
        {
            graphics->SetRenderTargetViews(SahdowRTVS, shadowDepthView);
            graphics->ClearDepthStencilView(shadowDepthView, static_cast<DepthStencilView::ClearFlags>(Flag), 1.0f, 0);
        }
        else
        {
            graphics->SetRenderTargetViews(SahdowRTVS, depthStencilView);
            graphics->ClearDepthStencilView(depthStencilView, static_cast<DepthStencilView::ClearFlags>(Flag), 1.0f, 0);
        }

        graphics->SetVertexShader(p_ShadowVertexshader);
        graphics->SetPixelShader(p_ShadowPixelShader);
        graphics->SetTopology(p_topology);
        graphics->SetVertexBuffer(p_pistolVertexBuffer, sizeof(SimpleVertex), 0);
        graphics->SetIndexBuffer(p_pistolIndexBuffer);
        graphics->SetConstantBuffer(constantBuffer);
        graphics->SetSampler(0, mySampler);

        if (normalTexture) graphics->SetTexture2D(2, normalTexture);
        if (albedoTexture) graphics->SetTexture2D(3, albedoTexture);
        if (specularTexture) graphics->SetTexture2D(4, specularTexture);

        // Clear render target used for shadow (if present)
        graphics->ClearRenderTargetView(ShadowRenderTarget, blackColor);

        graphics->SetRasterizerState(shadowRasterizer);
        graphics->Draw(pistolIndexCount, 0);


        graphics->SetVertexBuffer(p_cubeVertexBuffer, sizeof(SimpleVertex), 0);
        graphics->SetIndexBuffer(p_cubeIndexBuffer);
        graphics->SetConstantBuffer(constantBuffer);
        graphics->Draw(cubeIndexCount, 0);

        //if ( shadowDepthView)
        //{

        //    float bias = 0.005f;
        //    mathfu::Vector<float, 3> sampleWorldPos = mathfu::Vector<float, 3>(100.0f, 200.0f, 0.0f);
        //    bool occluded = graphics->IsOccluded(shadowDepthView, sampleWorldPos, cameraData.ShadowView, cameraData.ShadowProjection, bias);

        //    if (occluded)
        //    {
        //        std::cout << "Sample point is occluded by shadow map\n";
        //    }
        //    else
        //    {
        //        std::cout << "Sample point is lit\n";
        //    }
        //}

        graphics->SetShadowMapFromDepthView(5, shadowDepthView);

 // PASS 1: model and plane

        cameraData.worldMatrix = cameraData.worldMatrix * mathfu::Matrix<float, 4>::FromRotationMatrix(mathfu::Matrix<float, 4, 4>::RotationX(time * .5));
        graphics->UpdateConstantBuffer(constantBuffer, sizeof(GeneralConstBuffer), &cameraData);


            graphics->SetRenderTargetViews(gbufferRTVs, depthStencilView);

            graphics->ClearRenderTargetView(NormalRTV, clearColor);
            graphics->ClearRenderTargetView(ColorRTV, clearColor);
            graphics->ClearRenderTargetView(SpecularRTV, clearColor);
            graphics->ClearDepthStencilView(depthStencilView, static_cast<DepthStencilView::ClearFlags>(Flag), 1.0f, 0);

            graphics->SetVertexShader(p_vertexShader_1);
            graphics->SetPixelShader(p_pixelShader_1);

            graphics->SetTopology(p_topology);
            graphics->SetVertexBuffer(p_pistolVertexBuffer, sizeof(SimpleVertex), 0);
            graphics->SetIndexBuffer(p_pistolIndexBuffer);
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

            graphics->Draw(pistolIndexCount, 0);

            //update const buffer with the cube world matrix
            cameraData.worldMatrix = cubeWorldMatrix;
            graphics->UpdateConstantBuffer(constantBuffer, sizeof(GeneralConstBuffer), &cameraData);

            if (cubeTexture) graphics->SetTexture2D(3, cubeTexture);
            graphics->SetVertexBuffer(p_cubeVertexBuffer, sizeof(SimpleVertex), 0);
            graphics->SetIndexBuffer(p_cubeIndexBuffer);
            graphics->Draw(cubeIndexCount, 0);


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
