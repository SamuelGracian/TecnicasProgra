#include "DX11GraphicsAPI.h"

#include <memory>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

DX11GraphicsAPI::DX11GraphicsAPI() {}

bool DX11GraphicsAPI::Init(std::weak_ptr<Window> handleWindow)
{
  // Aquí va la inicialización de DirectX 11
  if (handleWindow.expired())
  {
    return false;
  }

  std::shared_ptr<Window> window = handleWindow.lock();

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

  // Create Swap Chain here

  return true;
}