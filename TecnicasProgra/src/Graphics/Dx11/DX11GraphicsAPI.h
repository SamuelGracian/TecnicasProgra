#include "GraphicsAPI.h"
#include <d3d11.h>
// Aquí si puedes inlcuir cosas de DirectX11

class DX11GraphicsAPI : public GRAPI
{
public:
  DX11GraphicsAPI();

  bool Init(std::weak_ptr<DisplaySurface> handleWindow) override;

 private:
  ID3D11Device* m_device = nullptr;
  ID3D11DeviceContext* m_immediateContext = nullptr;
};