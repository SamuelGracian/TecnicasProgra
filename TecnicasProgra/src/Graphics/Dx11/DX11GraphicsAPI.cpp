#include "DX11GraphicsAPI.h"

#include <memory>
#include <assert.h>
#include <iostream>

#include "Graphics/Dx11/Dx11SwapChain.h"
#include "Graphics/Dx11/Dx11ConstantBuffer.h"
#include "Graphics/Dx11/Dx11IndexBuffer.h"
#include "Graphics/Dx11/Dx11VertexBuffer.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
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

DX11GraphicsAPI::DX11GraphicsAPI() {}

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
  createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

  D3D_DRIVER_TYPE driverTypes[] =
      {
          D3D_DRIVER_TYPE_HARDWARE,
          D3D_DRIVER_TYPE_WARP,
          D3D_DRIVER_TYPE_REFERENCE,
      };
  UINT numDriverTypes = ARRAYSIZE(driverTypes);

  D3D_FEATURE_LEVEL featureLevels[] =
      {
          D3D_FEATURE_LEVEL_11_1,
          D3D_FEATURE_LEVEL_11_0,
          D3D_FEATURE_LEVEL_10_1,
          D3D_FEATURE_LEVEL_10_0,
      };
  UINT numFeatureLevels = ARRAYSIZE(featureLevels);

  HRESULT hr = S_OK;
  for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
  {
    D3D_DRIVER_TYPE g_driverType = driverTypes[driverTypeIndex];
    /// DELETE THIS
    D3D_FEATURE_LEVEL g_featureLevel = D3D_FEATURE_LEVEL_11_0;

    hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                           D3D11_SDK_VERSION, &m_device, &g_featureLevel, &m_immediateContext);

    if (hr == E_INVALIDARG)
    {
      // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
      hr = D3D11CreateDevice(nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                             D3D11_SDK_VERSION, &m_device, &g_featureLevel, &m_immediateContext);
    }

    if (SUCCEEDED(hr))
      break;
  }
  assert(!FAILED(hr));

  // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
  IDXGIFactory1 *dxgiFactory1 = nullptr;
  {
    IDXGIDevice *dxgiDevice = nullptr;
    hr = m_device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void **>(&dxgiDevice));
    if (SUCCEEDED(hr))
    {
      IDXGIAdapter *adapter = nullptr;
      hr = dxgiDevice->GetAdapter(&adapter);
      if (SUCCEEDED(hr))
      {
        hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void **>(&dxgiFactory1));
        adapter->Release();
      }
      dxgiDevice->Release();
    }
  }
  assert(!FAILED(hr));


  return true;
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

std::shared_ptr<SwapChain> DX11GraphicsAPI::CreateSwapChain(std::weak_ptr<DisplaySurface> handleWindow, uint32_t width, uint32_t height, GAPI_FORMAT::K format)
{
    std::shared_ptr <SwapChain> SChain = nullptr;

    if (handleWindow.expired())
    {
        std::cout << "Handle window expired" << std::endl;
        return SChain;
    }

    std::shared_ptr<DisplaySurface> tempSurface = handleWindow.lock();

    if (auto* ResultSwapChain = CreateSwapChain_internal (tempSurface->GetHandle(), width, height, format))
    {
        if (auto* ResultRT = CreateBackBufferRT_internal(ResultSwapChain))
        {
            SChain = std::make_shared<Dx11SwapChain>();

            std::shared_ptr<Dx11SwapChain> tempSwapChain = std::reinterpret_pointer_cast<Dx11SwapChain>(SChain);

            tempSwapChain->m_BackBUfferRT = ResultRT;

            tempSwapChain->m_swapChain = ResultSwapChain;
        }
    }
    return SChain;
}

void DX11GraphicsAPI::ClearSwapChain(std::weak_ptr<SwapChain> swapChain)
{
    float Color[4] = { 1,0,0,1 };

    if (swapChain.expired())
    {
        std::cout << "Expired Swap Chain" << std::endl;
        return;
    }
    std::shared_ptr<Dx11SwapChain> TempSwapChain = std::reinterpret_pointer_cast<Dx11SwapChain> (swapChain.lock());

    m_immediateContext->ClearRenderTargetView(TempSwapChain->m_BackBUfferRT, Color);
}

std::shared_ptr<ConstantBuffer> DX11GraphicsAPI::CreateConstantBuffer(const uint32_t bytewidth, const uint32_t slot, void* data)
{
    assert(bytewidth != 0);
    ID3D11Buffer* Rawbuffer = nullptr;
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = bytewidth;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;

    assert(!FAILED(m_device->CreateBuffer(&bd, nullptr, &Rawbuffer)));

    auto buffer = std::make_shared<Dx11ConstantBuffer>();
    buffer->m_buffer = Rawbuffer;
    buffer->SetByteWidth(bytewidth);
    buffer->SetSlot(slot);

    ASSIGN_DEBUG_NAME(buffer.get(), Rawbuffer);
    return buffer;
}

std::shared_ptr<IndexBuffer> DX11GraphicsAPI::CreateIndexBuffer(const uint32_t bytewidth, void* data, uint32_t indexcount)
{
    assert(bytewidth != 0);
    D3D11_BUFFER_DESC bd = {};
    ID3D11Buffer* Rawbuffer = nullptr;
    D3D11_SUBRESOURCE_DATA InitData = {};

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = bytewidth;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = data;

    assert(!FAILED(m_device->CreateBuffer(&bd, &InitData, &Rawbuffer)));

    auto buffer = std::make_shared<Dx11IndexBuffer>();
    buffer->m_buffer = Rawbuffer;
    return buffer;
}

std::shared_ptr<VertexBuffer> DX11GraphicsAPI::CreateVertexBuffer(const uint32_t bytewidth, const void* vertices)
{
    ID3D11Buffer* Rawbuffer = nullptr;

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = bytewidth;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = vertices;

    assert(!FAILED(m_device->CreateBuffer(&bd, &InitData, &Rawbuffer)));

    auto buffer = std::make_shared<Dx11VertexBuffer>();
    buffer->m_buffer = Rawbuffer;
    return buffer;
}
