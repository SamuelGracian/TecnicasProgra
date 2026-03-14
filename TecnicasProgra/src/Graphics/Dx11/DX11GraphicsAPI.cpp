#pragma once
#include "DX11GraphicsAPI.h"

#include <assert.h>
#include <iostream>
#include <d3dcompiler.h>
#include <vector>
#include <string>
#include <map>

#include "Graphics/Dx11/Dx11SwapChain.h"
#include "Graphics/Dx11/Dx11ConstantBuffer.h"
#include "Graphics/Dx11/Dx11IndexBuffer.h"
#include "Graphics/Dx11/Dx11VertexBuffer.h"
#include "Graphics/Dx11/Dx11Topology.h"
#include "Graphics/Dx11/Dx11VertexShader.h"
#include "Graphics/Dx11/Dx11PixelShader.h"
#include "Graphics/Dx11/Dx11DepthStencil.h"
#include "Graphics/Dx11/Dx11RenderTargetView.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
//#pragma comment(lib, "dxgi1_2.lib")

#define SAFE_RELEASE(x) if (x) {x -> Release(); x = nullptr;}

#if !defined(ASSIGN_DEBUG_NAME)
#   define ASSIGN_DEBUG_NAME(t, a)                                                                  \
   if (a)                                                                                          \
    {                                                                                               \
        std::string n = typeid(t).name();                                                                \
        a->SetPrivateData(WKPDID_D3DDebugObjectName, static_cast<uint32_t>(n.size()), n.c_str());     \
    }
#endif

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
        m_immediateContext->Draw(vertexCount, startVertexLocation);
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