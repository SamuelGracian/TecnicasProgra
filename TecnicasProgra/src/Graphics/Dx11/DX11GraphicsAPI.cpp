#pragma once
#include "DX11GraphicsAPI.h"

#include <wincodec.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assert.h>
#include <iostream>
#include <d3dcompiler.h>
#include <vector>
#include <string>
#include <map>
#include <fstream>
#include <unordered_map>
#include <sstream>

#include "Graphics/Dx11/Dx11ConstantBuffer.h"
#include "Graphics/Dx11/Dx11SwapChain.h"
#include "Graphics/Dx11/Dx11IndexBuffer.h"
#include "Graphics/Dx11/Dx11VertexBuffer.h"
#include "Graphics/Dx11/Dx11Topology.h"
#include "Graphics/Dx11/Dx11VertexShader.h"
#include "Graphics/Dx11/Dx11PixelShader.h"
#include "Graphics/Dx11/Dx11DepthStencil.h"
#include "Graphics/Dx11/Dx11RenderTargetView.h"
#include "Graphics/Dx11/Dx11Texture2d.h"
#include "Graphics/Dx11/Dx11SamplerState.h" 
#include "Main/stb_image.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "windowscodecs.lib")


#define SAFE_RELEASE(x) if (x) {x -> Release(); x = nullptr;}

#define STB_IMAGE_IMPLEMENTATION
#include "Main/stb_image.h"

#if !defined(ASSIGN_DEBUG_NAME)
#   define ASSIGN_DEBUG_NAME(t, a)                                                                  \
   if (a)                                                                                          \
    {                                                                                               \
        std::string n = typeid(t).name();                                                                \
        a->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<uint32_t>(n.size()), n.c_str());     \
    }
#endif


using Vector3 = mathfu::Vector<float, 3>;
Vector3 position;
mathfu::Vector < float, 2> Vector2;

//* //////////////////////////// HELPERS
DXGI_FORMAT GetDX11Format_internal(const GAPI_FORMAT::K format)
{
    switch (format)
    {
    default:
        return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
        break;

    case GAPI_FORMAT::FORMAT_D24_UNORM_S8_UINT:
        return DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
        break;

    case GAPI_FORMAT::FORMAT_R8G8B8A8_UNORM:
        return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
    }
}

uint32_t GetDx11BindFlag_internal(uint32_t bindFlags)
{
    uint32_t ResultFlags = 0;

    if ((bindFlags & GAPI_BIND_FLAGS::BIND_CONSTANT_BUFFER) != 0)
    {
        ResultFlags += D3D11_BIND_FLAG::D3D11_BIND_CONSTANT_BUFFER;
    }

    if ((bindFlags & GAPI_BIND_FLAGS::BIND_INDEX_BUFFER) != 0)
    {
        ResultFlags += D3D11_BIND_INDEX_BUFFER;
    }

    if ((bindFlags & GAPI_BIND_FLAGS::BIND_VERTEX_BUFFER) != 0)
    {
        ResultFlags += D3D11_BIND_VERTEX_BUFFER;
    }

    if ((bindFlags & GAPI_BIND_FLAGS::DEPTH_STENCIL) != 0)
    {
        ResultFlags += D3D11_BIND_DEPTH_STENCIL;
    }

    if ((bindFlags & GAPI_BIND_FLAGS::RENDER_TARGET) != 0)
    {
        ResultFlags += D3D11_BIND_RENDER_TARGET;
    }

    if ((bindFlags & GAPI_BIND_FLAGS::SHADER_RESOURCE) != 0)
    {
        ResultFlags += D3D11_BIND_SHADER_RESOURCE;
    }

    return ResultFlags;
}

const char* GetShaderModel_internal(SHADER_TYPE::K shaderType, uint32_t shaderModel)
{
    static std::string ResultShader;

    std::string modelVersion = std::to_string(shaderModel) + "_0";

    switch (shaderType)
    {
    case SHADER_TYPE::K::VERTEX_SHADER:
        ResultShader = "vs_" + modelVersion;
        break;

    case SHADER_TYPE::K::PIXEL_SHADER:
        ResultShader = "ps_" + modelVersion;
        break;

    default:
        ResultShader = "vs_4_0";
        break;
    }

    return ResultShader.c_str();
}

namespace Dx11HELPERS
{
    DXGI_FORMAT GetDX11Format_internal(const GAPI_FORMAT::K format)
    {
        switch (format)
        {
        default:
            return DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;
            break;

        case GAPI_FORMAT::FORMAT_D24_UNORM_S8_UINT:
            return DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
            break;

        case GAPI_FORMAT::FORMAT_R8G8B8A8_UNORM:
            return DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
        }
    }
}

std::vector<D3D11_INPUT_ELEMENT_DESC> CreateInputLayoutDesc_internal(ID3D11ShaderReflection* reflection)
{
    
    std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;

    if (reflection)
    {
        D3D11_SHADER_DESC shaderDesc;
        reflection->GetDesc(&shaderDesc);

        inputLayoutDesc.reserve(shaderDesc.InputParameters);

        for (UINT i = 0; i < shaderDesc.InputParameters; ++i)
        {
            D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
            reflection->GetInputParameterDesc(i, &paramDesc);

            D3D11_INPUT_ELEMENT_DESC elementDesc = {};
            elementDesc.SemanticName = paramDesc.SemanticName;
            elementDesc.SemanticIndex = paramDesc.SemanticIndex;
            elementDesc.InputSlot = 0;
            elementDesc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
            elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
            elementDesc.InstanceDataStepRate = 0;

            UINT componentCount = 0;
            if (paramDesc.Mask == 1) componentCount = 1;
            else if (paramDesc.Mask <= 3) componentCount = 2;
            else if (paramDesc.Mask <= 7) componentCount = 3;
            else if (paramDesc.Mask <= 15) componentCount = 4;

            if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
            {
                if (componentCount == 1) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
                else if (componentCount == 2) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
                else if (componentCount == 3) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
                else if (componentCount == 4) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            }
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
            {
                if (componentCount == 1) elementDesc.Format = DXGI_FORMAT_R32_UINT;
                else if (componentCount == 2) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
                else if (componentCount == 3) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
                else if (componentCount == 4) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
            }
            else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
            {
                if (componentCount == 1) elementDesc.Format = DXGI_FORMAT_R32_SINT;
                else if (componentCount == 2) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
                else if (componentCount == 3) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
                else if (componentCount == 4) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
            }

            inputLayoutDesc.push_back(elementDesc);
        }
    }
    return inputLayoutDesc;
}


//* ///////////////////////////////////////////////////////

DX11GraphicsAPI::DX11GraphicsAPI() 
{

    //TO DO: Cambiar a parametro
    m_shaderModel = 5;
}

bool DX11GraphicsAPI::Init(std::weak_ptr<DisplaySurface> handleWindow)
{
  // Aquí va la inicialización de DirectX 11
  if (handleWindow.expired())
  {
    return false;
  }

  std::shared_ptr<DisplaySurface> window = handleWindow.lock();

  // Create Device and Device Context here

  UINT createDeviceFlags = 0;
#ifdef _DEBUG
  createDeviceFlags |= (D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_DEBUGGABLE);
#endif

  std::vector <D3D_DRIVER_TYPE> driverTypes =
      {
          D3D_DRIVER_TYPE_HARDWARE,
          D3D_DRIVER_TYPE_WARP,
          D3D_DRIVER_TYPE_REFERENCE,
      };

  std::vector <D3D_FEATURE_LEVEL> featureLevels =
      {
          D3D_FEATURE_LEVEL_11_1,
          D3D_FEATURE_LEVEL_11_0,
          D3D_FEATURE_LEVEL_10_1,
          D3D_FEATURE_LEVEL_10_0,
      };

  D3D_FEATURE_LEVEL resultFeatureLevel;
  
  for ( const auto& drivertype: driverTypes )
  {
 
    if (SUCCEEDED(D3D11CreateDevice(nullptr, drivertype, nullptr, createDeviceFlags, featureLevels.data(), featureLevels.size(),
        D3D11_SDK_VERSION, &m_device, &resultFeatureLevel , &m_immediateContext)))
    {

        D3D11_VIEWPORT viewport;
        ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = window->GetWidth();
        viewport.Height = window->GetHeight();

        m_immediateContext->RSSetViewports(1, &viewport);

        return true;
    }

  }

  return false;
}

void DX11GraphicsAPI::CleanUpResources()
{
}

IDXGISwapChain* DX11GraphicsAPI::CreateSwapChain_internal(HWND hwnd, uint32_t width, uint32_t height, GAPI_FORMAT::K format)
{
    IDXGIFactory1* dxgiFactory1 = nullptr;

    IDXGIFactory2* dxgiFactory2 = nullptr;

    IDXGIDevice* dxgiDevice = nullptr;

    IDXGIAdapter* adapter = nullptr;

    IDXGISwapChain1* swapChain1 = nullptr;

    IDXGISwapChain* ResultSwapChain = nullptr;

    if (SUCCEEDED(m_device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice))))
    {
        if (SUCCEEDED(dxgiDevice->GetAdapter(&adapter)))
        {
            HRESULT hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory1));

            if ( SUCCEEDED (hr) )
            {
                if (dxgiFactory1->QueryInterface(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2)))
                {

                    DXGI_SWAP_CHAIN_DESC1 sd = {};
                    sd.Width = width;
                    sd.Height = height;
                    sd.Format = Dx11HELPERS:: GetDX11Format_internal(format);
                    sd.SampleDesc.Count = 1;
                    sd.SampleDesc.Quality = 0;
                    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                    sd.BufferCount = 1;


                    if (SUCCEEDED(dxgiFactory2->CreateSwapChainForHwnd(m_device, hwnd, &sd, nullptr, nullptr, &swapChain1)))
                    {
                        swapChain1->QueryInterface(__uuidof(IDXGISwapChain), reinterpret_cast<void**>(&ResultSwapChain));
                    }
                }
                else
                {
                    DXGI_SWAP_CHAIN_DESC sd = {};
                    sd.BufferCount = 1;
                    sd.BufferDesc.Width = width;
                    sd.BufferDesc.Height = height;
                    sd.BufferDesc.Format = Dx11HELPERS::GetDX11Format_internal(format);
                    sd.BufferDesc.RefreshRate.Numerator = 60;
                    sd.BufferDesc.RefreshRate.Denominator = 1;
                    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
                    sd.OutputWindow = hwnd;
                    sd.SampleDesc.Count = 1;
                    sd.SampleDesc.Quality = 0;
                    sd.Windowed = TRUE;


                    dxgiFactory1->CreateSwapChain(m_device, &sd, &ResultSwapChain);
                }

                dxgiFactory1->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);
            }
        }
    }

    SAFE_RELEASE(dxgiFactory1);
    SAFE_RELEASE(dxgiFactory2);
    SAFE_RELEASE(dxgiDevice);
    SAFE_RELEASE(adapter);
    SAFE_RELEASE(swapChain1);

    return ResultSwapChain;
}

ID3D11RenderTargetView* DX11GraphicsAPI::CreateBackBufferRT_internal(IDXGISwapChain* swapChain)
{
    ID3D11Texture2D* ResultTextureRt = nullptr;

    ID3D11RenderTargetView* ResultRT = nullptr;

    if (swapChain != nullptr && m_device != nullptr)
    {
        if (SUCCEEDED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&ResultTextureRt))))
        {
            m_device->CreateRenderTargetView(ResultTextureRt, nullptr, &ResultRT);
        }
    }
    SAFE_RELEASE(ResultTextureRt);

    return ResultRT;
}

ID3DBlob* DX11GraphicsAPI::CompileShader_internal(const std::string& shaderCode, const std::string& entrypoint, std::vector<std::string> Defines, SHADER_TYPE::K shaderType)
{
        ID3DBlob* ErrorBlob = nullptr;
        ID3DBlob* BinaryBlob = nullptr;

    if (!shaderCode.empty() && !entrypoint.empty())
    {

        std::string FinalShaderCode;


        uint32_t dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
        // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
        // Setting this flag improves the shader debugging experience, but still allows 
        // the shaders to be optimized and to run exactly the way they will run in 
        // the release configuration of this program.
        dwShaderFlags |= D3DCOMPILE_DEBUG;

        // Disable optimizations to further improve shader debugging
        dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        for (auto& macro : Defines)
        {
            FinalShaderCode += (macro + '\n');
        }

        FinalShaderCode += shaderCode;

        if (FAILED(D3DCompile(FinalShaderCode.c_str(), FinalShaderCode.length(), nullptr, nullptr, nullptr,
            entrypoint.c_str(), GetShaderModel_internal(shaderType, m_shaderModel), dwShaderFlags, 0, &BinaryBlob, &ErrorBlob)))
        {
            if (ErrorBlob)
            {
                std::cout << "Shader Compilation Error: " << reinterpret_cast<const char*>(ErrorBlob->GetBufferPointer()) << std::endl;
            }
        }
    }

    SAFE_RELEASE(ErrorBlob);
    return BinaryBlob;
}

ID3D11Texture2D* DX11GraphicsAPI::CreateTexture2D_internal(uint32_t width, uint32_t height, const GAPI_FORMAT::K format, uint32_t bindFlags)
{
    ID3D11Texture2D* ResultTexture = nullptr;
   if (width != 0 && height != 0 && format != GAPI_FORMAT::FORMAT_UNKNOWN && m_device != nullptr)
   {

       D3D11_TEXTURE2D_DESC descTexture = {};
       descTexture.Width = width;
       descTexture.Height = height;
       descTexture.MipLevels = 1;
       descTexture.ArraySize = 1;
       descTexture.Format = GetDX11Format_internal(format);
       descTexture.SampleDesc.Count = 1;
       descTexture.SampleDesc.Quality = 0;
       descTexture.Usage = D3D11_USAGE_DEFAULT;
       descTexture.BindFlags = GetDx11BindFlag_internal(bindFlags);
       descTexture.CPUAccessFlags = 0;
       descTexture.MiscFlags = 0;
       m_device->CreateTexture2D(&descTexture, nullptr, &ResultTexture);
   }

    return ResultTexture;
}



std::shared_ptr<SwapChain> DX11GraphicsAPI::CreateSwapChain(std::weak_ptr<DisplaySurface> handleWindow, GAPI_FORMAT::K format)
{
    std::shared_ptr <Dx11SwapChain> SChain = nullptr;

    if (handleWindow.expired())
    {
        std::cout << "Handle window expired" << std::endl;
        return SChain;
    }

    std::shared_ptr<DisplaySurface> tempSurface = handleWindow.lock();

    if (auto* ResultSwapChain = CreateSwapChain_internal (tempSurface->GetHandle(),tempSurface->GetClientWidth(), tempSurface->GetClientHeight(), format))
    {
        if (auto* ResultRT = CreateBackBufferRT_internal(ResultSwapChain))
        {
            SChain = std::make_shared<Dx11SwapChain>();

            SChain->m_BackBufferRT = ResultRT;

            SChain->m_swapChain = ResultSwapChain;
        }
    }
    return SChain;
}

std::shared_ptr<ConstantBuffer> DX11GraphicsAPI::CreateConstantBuffer(const uint32_t bytewidth, const uint32_t slot, void* data)
{
    ID3D11Buffer* Rawbuffer = nullptr;
    D3D11_BUFFER_DESC bd = {};
    if (bytewidth != 0)
    {
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = bytewidth;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
    }

    if (SUCCEEDED( (m_device->CreateBuffer(&bd, nullptr, &Rawbuffer))))

    {
        auto buffer = std::make_shared<Dx11ConstantBuffer>();
        buffer->m_buffer = Rawbuffer;
        buffer->SetByteWidth(bytewidth);
        buffer->SetSlot(slot);

        ASSIGN_DEBUG_NAME(buffer.get(), Rawbuffer);
        return buffer;
    }
    return nullptr;
}

std::shared_ptr<IndexBuffer> DX11GraphicsAPI::CreateIndexBuffer(const uint32_t bytewidth, void* data, uint32_t indexcount)
{
    assert(bytewidth != 0);
    D3D11_BUFFER_DESC bd = {};
    ID3D11Buffer* Rawbuffer = nullptr;
    D3D11_SUBRESOURCE_DATA InitData = {};
    auto ResultBuffer = std::make_shared<Dx11IndexBuffer>();

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = bytewidth;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = data;

    if (SUCCEEDED( m_device->CreateBuffer(&bd, &InitData, &Rawbuffer)))
    {
        ResultBuffer->m_buffer = Rawbuffer;
    }

    return ResultBuffer;
}

std::shared_ptr<VertexBuffer> DX11GraphicsAPI::CreateVertexBuffer(const uint32_t bytewidth, const void* vertices)
{
    ID3D11Buffer* Rawbuffer = nullptr;
    std::shared_ptr<Dx11VertexBuffer> ResultBuffer;

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = bytewidth;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.StructureByteStride = 24;

    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = vertices;

   if (SUCCEEDED(m_device->CreateBuffer(&bd, &InitData, &Rawbuffer)))
   {
       ResultBuffer = std::make_shared<Dx11VertexBuffer>();
       ResultBuffer->m_buffer = Rawbuffer;
   }
    return ResultBuffer;
}

void DX11GraphicsAPI::SetConstantBuffer(std::weak_ptr<ConstantBuffer> buffer)
{
    if (m_immediateContext == nullptr || buffer.expired())
    {
        return;
    }
    auto pbuffer = std::static_pointer_cast<Dx11ConstantBuffer>(buffer.lock());

    if (pbuffer == nullptr || pbuffer->m_buffer == nullptr || pbuffer->GetSlot() >= HIGHER_AVAILABLE_SLOT)
    {
        return;
    }

    m_immediateContext->VSSetConstantBuffers(pbuffer->GetSlot(), 1, &pbuffer->m_buffer);
    m_immediateContext->PSSetConstantBuffers(pbuffer->GetSlot(), 1, &pbuffer->m_buffer);
}

void DX11GraphicsAPI::SetIndexBuffer(std::weak_ptr<IndexBuffer> buffer)
{
    if (m_immediateContext == nullptr || buffer.expired())
    {
        return;
    }
    
    auto pbuffer = std::static_pointer_cast<Dx11IndexBuffer> (buffer.lock());
    
    if (pbuffer == nullptr || pbuffer->m_buffer == nullptr)
    {
        return;
    }

    m_immediateContext->IASetIndexBuffer(pbuffer->m_buffer, DXGI_FORMAT_R16_UINT, 0);
}

void DX11GraphicsAPI::UpdateConstantBuffer(std::weak_ptr<ConstantBuffer> buffer, const uint32_t bytewidth, void* Data)
{

    if (m_immediateContext == nullptr || buffer.expired() || Data == nullptr)
    {
        return;
    }
    auto pbuffer = std::static_pointer_cast <Dx11ConstantBuffer> (buffer.lock());

    if (pbuffer == nullptr || pbuffer->m_buffer == nullptr || pbuffer->GetByteWidth() != bytewidth)
    {
        return;
    }

    m_immediateContext->UpdateSubresource(pbuffer->m_buffer, 0, nullptr, Data, 0, 0);
}

std::shared_ptr<Topology> DX11GraphicsAPI::CreateTopology(Topology::Type type)
{
    auto topo = std::make_shared<Dx11Topology>();
    // set base type and DX primitive accordingly
    topo->SetType(type);

    switch (type)
    {
    case Topology::Type::TriangleList:
        topo->m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        break;
    case Topology::Type::TriangleStrip:
        topo->m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
        break;
    case Topology::Type::LineList:
        topo->m_topology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
        break;
    case Topology::Type::PointList:
        topo->m_topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
        break;
    default:
        topo->m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        break;
    }

    return topo;
}

void DX11GraphicsAPI::SetTopology(std::weak_ptr<Topology> topology)
{
    if (m_immediateContext == nullptr || topology.expired())
    {
        return;
    }

    auto p_topology = std::static_pointer_cast<Dx11Topology>(topology.lock());
    if (p_topology == nullptr)
    {
        return;
    }

    m_immediateContext->IASetPrimitiveTopology(p_topology->m_topology);
}

std::shared_ptr<VertexShader> DX11GraphicsAPI::CreateVertexShader(const std::string& shaderCode, const std::string& entrypoint, std::vector<std::string> Defines)
{
    ID3DBlob* BinaryBlob = nullptr;

    ID3D11VertexShader* ResultShader = nullptr;

    ID3D11InputLayout* resultLayout = nullptr;

    ID3D11ShaderReflection* shaderReflection = nullptr;

    BinaryBlob = CompileShader_internal(shaderCode, entrypoint, Defines, SHADER_TYPE::K::VERTEX_SHADER);

    std::shared_ptr<Dx11VertexShader> p_VS = nullptr;

    if (BinaryBlob != nullptr && m_device != nullptr)
    {
        if (SUCCEEDED(D3DReflect(
            BinaryBlob->GetBufferPointer(),
            BinaryBlob->GetBufferSize(),
            IID_ID3D11ShaderReflection,
            (void**)&shaderReflection)))
        {
            
            const auto desc = CreateInputLayoutDesc_internal(shaderReflection);

            if (!desc.empty())
            {
                if (SUCCEEDED(m_device->CreateVertexShader(BinaryBlob->GetBufferPointer(), BinaryBlob->GetBufferSize(), nullptr, &ResultShader)) &&
                    SUCCEEDED(m_device->CreateInputLayout(desc.data(), desc.size(), BinaryBlob->GetBufferPointer(), BinaryBlob->GetBufferSize(), &resultLayout)))
                {
                    p_VS = std::make_shared<Dx11VertexShader>();
                    p_VS->m_shader = ResultShader;
                    p_VS->m_InputLayout = resultLayout;
                }
            }
        }
    }
    SAFE_RELEASE (shaderReflection)
    SAFE_RELEASE(BinaryBlob);
    return p_VS;
}

std::shared_ptr<PixelShader> DX11GraphicsAPI::CreatePixelShader(const std::string& shaderCode, const std::string& entrypoint, std::vector<std::string> Defines)
{
    ID3DBlob* BinaryBlob = nullptr;

    ID3D11PixelShader* ResultShader = nullptr;

    std::shared_ptr<Dx11PixelShader> shaderPtr = nullptr;

    BinaryBlob = CompileShader_internal(shaderCode, entrypoint, Defines, SHADER_TYPE::K::PIXEL_SHADER);

    if (BinaryBlob != nullptr)
    {
      if (SUCCEEDED( m_device->CreatePixelShader(BinaryBlob->GetBufferPointer(), BinaryBlob->GetBufferSize(), nullptr, &ResultShader)))
      {
        shaderPtr = std::make_shared<Dx11PixelShader>();
        shaderPtr->m_shader = ResultShader;
      }
    }
    SAFE_RELEASE(BinaryBlob);
    return shaderPtr;
}

void DX11GraphicsAPI::SetVertexShader(std::weak_ptr<VertexShader> shader)
{
    if (shader.expired())
    {
        return;
    }

    auto p_VS = std::reinterpret_pointer_cast<Dx11VertexShader>(shader.lock());

    if (p_VS && m_immediateContext)
    {
        m_immediateContext->IASetInputLayout(p_VS->m_InputLayout);
        m_immediateContext->VSSetShader(p_VS->m_shader, nullptr, 0);
    }
}

void DX11GraphicsAPI::SetPixelShader(std::weak_ptr<PixelShader> shader)
{
    if (shader.expired())
    {
        return;
    }

    auto p_PS = std::reinterpret_pointer_cast<Dx11PixelShader>(shader.lock());

    if (p_PS && m_immediateContext)
    {
        m_immediateContext->PSSetShader(p_PS->m_shader, nullptr, 0);
    }
}

void DX11GraphicsAPI::SetVertexBuffer(std::weak_ptr<VertexBuffer> buffer, uint32_t stride, uint32_t offset)
{
    if (buffer.expired() || m_immediateContext == nullptr)
    {
        return;
    }

    auto p_buffer = std::static_pointer_cast<Dx11VertexBuffer>(buffer.lock());

    if (p_buffer && p_buffer->m_buffer)
    {
        m_immediateContext->IASetVertexBuffers(0, 1, &p_buffer->m_buffer, &stride, &offset);
    }
}

void DX11GraphicsAPI::Draw(uint32_t vertexCount, uint32_t startVertexLocation)
{
    if (m_immediateContext)
    {
       // m_immediateContext->Draw(vertexCount, startVertexLocation);
        m_immediateContext->DrawIndexed(vertexCount, startVertexLocation, 0);
    }
}

std::shared_ptr<DepthStencilView> DX11GraphicsAPI::CreateDepthStencil(uint32_t width, uint32_t height, const GAPI_FORMAT::K format)
{
    std::shared_ptr<Dx11DepthStencilView>resultDepthStencil = nullptr;
    ID3D11DepthStencilView* ResultDepthStencilView = nullptr;
    ID3D11Texture2D* depthStencilTexture = nullptr;

    if (m_device != nullptr && width != 0 && height != 0 && format != GAPI_FORMAT::FORMAT_UNKNOWN)
    {
       
       if (depthStencilTexture = CreateTexture2D_internal(width, height, format, GAPI_BIND_FLAGS::DEPTH_STENCIL))
       {
         D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
         descDSV.Format = GetDX11Format_internal(format);
         descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
         descDSV.Texture2D.MipSlice = 0;

         if (SUCCEEDED(m_device->CreateDepthStencilView(depthStencilTexture, &descDSV, &ResultDepthStencilView)))    
         {
             resultDepthStencil = std::make_shared<Dx11DepthStencilView>();
             resultDepthStencil->m_depthStencilView = ResultDepthStencilView;
         }
       }
    }
    SAFE_RELEASE(depthStencilTexture);
    return resultDepthStencil;
}

std::shared_ptr<ViewPort> DX11GraphicsAPI::CreateViewPort(float width, float height, float minDepth, float maxDepth, float topLeftX, float topLeftY)
{
    return std::shared_ptr<ViewPort>();
}

void DX11GraphicsAPI::SetRenderTargetView(std::weak_ptr<RenderTargetView> renderTargetView, std::weak_ptr<DepthStencilView> depthStencilView)
{
    if (m_immediateContext == nullptr)
    {
        std::cout << "Null immediate context" << std::endl;
        return;
    }

    ID3D11RenderTargetView* rtv = nullptr;
    ID3D11DepthStencilView* dsv = nullptr;

    // Get RenderTargetView
    if (!renderTargetView.expired())
    {
        std::shared_ptr<Dx11RenderTargetView> tempRTV = std::reinterpret_pointer_cast<Dx11RenderTargetView>(renderTargetView.lock());
        if (tempRTV && tempRTV->m_renderTargetView)
        {
            rtv = tempRTV->m_renderTargetView;
        }
    }

    // Get DepthStencilView
    if (!depthStencilView.expired())
    {
        std::shared_ptr<Dx11DepthStencilView> tempDSV = std::reinterpret_pointer_cast<Dx11DepthStencilView>(depthStencilView.lock());
        if (tempDSV && tempDSV->m_depthStencilView)
        {
            dsv = tempDSV->m_depthStencilView;
        }
    }

    // Set both render target and depth stencil
    m_immediateContext->OMSetRenderTargets(1, &rtv, dsv);
}

void DX11GraphicsAPI::ClearRenderTargetView(std::weak_ptr<RenderTargetView> renderTargetView, float color[4])
{
    if (renderTargetView.expired() || m_immediateContext == nullptr )
    {
        std::cout << "Expired RenderTargetView or null context" << std::endl;
        return;
    }



    std::shared_ptr<Dx11RenderTargetView> tempRTV = std::reinterpret_pointer_cast<Dx11RenderTargetView>(renderTargetView.lock());

    if (tempRTV && tempRTV->m_renderTargetView)
    {
        m_immediateContext->ClearRenderTargetView(tempRTV->m_renderTargetView, color);
    }
}

void DX11GraphicsAPI::ClearDepthStencilView(std::weak_ptr<DepthStencilView> depthStencil, DepthStencilView::ClearFlags flag , float depth, uint32_t stencil)
{
    if (depthStencil.expired() || m_immediateContext == nullptr)
    {
        std::cout << "Expired Depth Stencil View" << std::endl;
        return;
    }

    std::shared_ptr<Dx11DepthStencilView> tempDS = std::reinterpret_pointer_cast<Dx11DepthStencilView>(depthStencil.lock());

    if (tempDS && m_immediateContext)
    {
        UINT  Dx11flag = Dx11DepthStencilView::GetClearFlagDx11(flag);

        m_immediateContext->ClearDepthStencilView(tempDS->m_depthStencilView, Dx11flag, depth, stencil);
    }

}

std::shared_ptr<Texture2D> DX11GraphicsAPI::CreateTexture2D(std::vector<uint8_t> imageData, int32_t width, int32_t height)
{
    auto texture = std::make_shared<Dx11Texture2D>();

    if (imageData.empty() || width <= 0 || height <= 0 || m_device == nullptr)
    {
        return nullptr;
    }

    D3D11_TEXTURE2D_DESC texDesc = {};
    texDesc.Width = static_cast<UINT>(width);
    texDesc.Height = static_cast<UINT>(height);
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; 
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA subData = {};
    subData.pSysMem = imageData.data();
    subData.SysMemPitch = static_cast<UINT>(width * 4);
    subData.SysMemSlicePitch = 0;

    HRESULT hr = m_device->CreateTexture2D(&texDesc, &subData, &texture->m_texture);

    if (SUCCEEDED(hr))
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = texDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;

        m_device->CreateShaderResourceView(texture->m_texture, &srvDesc, &texture->m_textureView);
    }
    else
    {
        std::cout << "Error loading texture to dx11." << std::endl;
        return nullptr;
    }
    return texture;
}

std::shared_ptr<SamplerState> DX11GraphicsAPI::CreateSamplerState()
{
    if (m_device == nullptr)
    {
        return nullptr;
    }

    auto samplerState = std::make_shared<Dx11SamplerState>();

    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;    
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;   
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;  
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

  
    ID3D11SamplerState* rawSampler = nullptr;
    HRESULT hr = m_device->CreateSamplerState(&sampDesc, &rawSampler);
    if (SUCCEEDED(hr))
    {
        samplerState->m_samplerState = rawSampler; 
        
        return samplerState;
    }

    return nullptr;
}

void DX11GraphicsAPI::SetSampler(uint32_t slot, std::weak_ptr<SamplerState> sampler)
{
    if (m_immediateContext == nullptr || sampler.expired())
    {
        return;
    }

    auto dx11Sampler = std::reinterpret_pointer_cast<Dx11SamplerState>(sampler.lock());
    
    if (dx11Sampler && dx11Sampler->m_samplerState)
    {
        m_immediateContext->PSSetSamplers(slot, 1, &dx11Sampler->m_samplerState);
    }
}

std::vector<uint8_t> DX11GraphicsAPI::LoadImageFromFile(const std::string& filepath, int32_t* width, int32_t* height, int32_t* channels)
{
    std::vector<uint8_t> imageData;

    uint8_t* data = stbi_load(filepath.c_str(), width, height, channels, STBI_rgb_alpha);

    if (data) {
        imageData.resize((*width) * (*height) * STBI_rgb_alpha);
        memcpy(imageData.data(), data, imageData.size());
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Error loading image: " << filepath << std::endl;
    }

    return imageData;
}

void DX11GraphicsAPI::SetTexture2D(uint32_t slot, std::weak_ptr<Texture2D> texture)
{
    if (m_immediateContext == nullptr || texture.expired())
    {
        return;
    }

    auto dx11Texture = std::static_pointer_cast<Dx11Texture2D>(texture.lock());

    if (dx11Texture && dx11Texture->m_textureView)
    {
        m_immediateContext->PSSetShaderResources(slot, 1, &dx11Texture->m_textureView);
    }
}


bool DX11GraphicsAPI::ImportModelAsset_Assimp(const std::string& filename, std::vector<SimpleVertex>& outVertices, std::vector<uint16_t>& outIndices)
{
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        filename,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

    if (!scene || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) || !scene->mRootNode)
    {
        std::cout << "Error loading model: " << importer.GetErrorString() << std::endl;
        return false;
    }

    for (unsigned int meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
    {
        aiMesh* mesh = scene->mMeshes[meshIndex];
        if (!mesh)
            continue;

        const uint32_t vertexOffset = static_cast<uint32_t>(outVertices.size());

        for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
        {
            SimpleVertex vertex;
            vertex.Pos = mathfu::Vector<float, 4>(
                mesh->mVertices[i].x,
                mesh->mVertices[i].y,
                mesh->mVertices[i].z,
                1.0f);

            if (mesh->mTextureCoords[0])
            {
                vertex.Tex = mathfu::Vector<float, 2>(
                    mesh->mTextureCoords[0][i].x,
                    mesh->mTextureCoords[0][i].y);
            }
            else
            {
                vertex.Tex = mathfu::Vector<float, 2>(0.0f, 0.0f);
            }

            outVertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
        {
            const aiFace& face = mesh->mFaces[i];

            for (unsigned int j = 0; j < face.mNumIndices; ++j)
            {
                uint32_t index = vertexOffset + face.mIndices[j];

                if (index > UINT16_MAX)
                {
                    std::cout << "Error: model has too many vertices for uint16_t indices." << std::endl;
                    return false;
                }

                outIndices.push_back(static_cast<uint16_t>(index));
            }
        }
    }

    return true;
}


//void DX11GraphicsAPI::ObjectLoader(const std::string& filename, std::vector<SimpleVertex>& outVertices, std::vector<uint16_t>& outIndices, std::string& outError)
//{
//    std::ifstream in(filename);
//    if (!in.is_open())
//    {
//        outError = "Failed to open: " + filename;
//        return;
//    }
//
//    Vector3 poition;
//    std::vector<Vector2> texCoords;
//    std::vector<SimpleVertex> vertices;
//    std::vector<uint32_t> indices32;
//    std::unordered_map<std::string, uint32_t> indexMap;
//
//    auto makeKey = [](int vi, int vti) -> std::string
//    {
//        return std::to_string(vi) + "/" + std::to_string(vti);
//    };
//
//    auto parseFaceRef = [](const std::string& ref, int& vi, int& vti)
//    {
//        vi = 0;
//        vti = 0;
//
//        const size_t firstSlash = ref.find('/');
//        if (firstSlash == std::string::npos)
//        {
//            vi = std::stoi(ref);
//            return;
//        }
//
//        const std::string posPart = ref.substr(0, firstSlash);
//        if (!posPart.empty())
//        {
//            vi = std::stoi(posPart);
//        }
//
//        const size_t secondSlash = ref.find('/', firstSlash + 1);
//        const std::string texPart = ref.substr(firstSlash + 1, secondSlash - firstSlash - 1);
//        if (!texPart.empty())
//        {
//            vti = std::stoi(texPart);
//        }
//    };
//
//    std::string line;
//    while (std::getline(in, line))
//    {
//        if (line.empty() || line[0] == '#')
//        {
//            continue;
//        }
//
//        std::istringstream iss(line);
//        std::string token;
//        iss >> token;
//
//        if (token == "v")
//        {
//            Vec3 p{};
//            iss >> p.x >> p.y >> p.z;
//            positions.push_back(p);
//        }
//        else if (token == "vt")
//        {
//            Vec2 t{};
//            iss >> t.u >> t.v;
//            texCoords.push_back(t);
//        }
//        else if (token == "f")
//        {
//            std::vector<std::string> refs;
//            std::string ref;
//            while (iss >> ref)
//            {
//                refs.push_back(ref);
//            }
//
//            if (refs.size() < 3)
//            {
//                continue;
//            }
//
//            auto processVertRef = [&](const std::string& faceRef) -> uint32_t
//            {
//                int vi = 0;
//                int vti = 0;
//                parseFaceRef(faceRef, vi, vti);
//
//                if (vi < 0) vi = static_cast<int>(positions.size()) + vi + 1;
//                if (vti < 0) vti = static_cast<int>(texCoords.size()) + vti + 1;
//
//                if (vi <= 0 || vi > static_cast<int>(positions.size()))
//                {
//                    outError = "OBJ: position index out of range in " + faceRef;
//                    throw std::runtime_error(outError);
//                }
//
//                const std::string key = makeKey(vi, vti);
//                auto it = indexMap.find(key);
//                if (it != indexMap.end())
//                {
//                    return it->second;
//                }
//
//                SimpleVertex simpleVrtx{};
//                const Vec3 p = positions[vi - 1];
//                simpleVrtx.Pos = Vec3 (p.x, p.y, p.z);
//
//                if (vti > 0 && vti <= static_cast<int>(texCoords.size()))
//                {
//                    const Vec2 t = texCoords[vti - 1];
//                    simpleVrtx.Tex = DirectX::XMFLOAT2(t.u, 1.0f - t.v);
//                }
//                else
//                {
//                    simpleVrtx.Tex = DirectX::XMFLOAT2(0.0f, 0.0f);
//                }
//
//                const uint32_t newIndex = static_cast<uint32_t>(vertices.size());
//                vertices.push_back(simpleVrtx);
//                indexMap.emplace(key, newIndex);
//                return newIndex;
//            };
//
//            try
//            {
//                for (size_t i = 1; i + 1 < refs.size(); ++i)
//                {
//                    const uint32_t a = processVertRef(refs[0]);
//                    const uint32_t b = processVertRef(refs[i]);
//                    const uint32_t c = processVertRef(refs[i + 1]);
//
//                    indices32.push_back(a);
//                    indices32.push_back(b);
//                    indices32.push_back(c);
//                }
//            }
//            catch (const std::exception& ex)
//            {
//                outError = ex.what();
//                return;
//            }
//        }
//    }
//
//    outVertices = std::move(vertices);
//    outIndices.clear();
//    outIndices.reserve(indices32.size());
//
//    for (uint32_t idx : indices32)
//    {
//        if (idx > UINT16_MAX)
//        {
//            outError = "OBJ: index exceeds uint16_t range";
//            outVertices.clear();
//            outIndices.clear();
//            return;
//        }
//
//        outIndices.push_back(static_cast<uint16_t>(idx));
//    }
//}
