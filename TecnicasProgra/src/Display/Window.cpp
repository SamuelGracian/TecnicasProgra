#include "Window.h"

#include <iostream>

#if defined(_WIN32)
static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}
#endif 

Window::~Window()
{
#if defined(_WIN32)
  DestroyWindow(m_hwnd);
#endif
}

bool Window::init(int width, int height, wchar_t* windowName)
{
#if defined(_WIN32)
  const char CLASS_NAME[] = "DX11WindowClass";
  WNDCLASSEX wc{};
  wc.cbSize = sizeof(wc);
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = WndProc;
  wc.hInstance = GetModuleHandle(nullptr);
  wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
  wc.lpszClassName = CLASS_NAME;
  RegisterClassEx(&wc);

  m_hwnd = CreateWindowEx(
      0,
      CLASS_NAME,
      "Tecnias Progra",
      WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 
      CW_USEDEFAULT, 
      width, height,
      nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

  if (!m_hwnd)
  {
    std::cout << "Window could not be created" << std::endl;

    return false;
  }

  ShowWindow(m_hwnd, SW_SHOW);
  UpdateWindow(m_hwnd);

  return true;
#else
  return false;
#endif
}

void Window::processMessages()
{
#if defined(_WIN32)
  MSG msg{};
  while (GetMessage(&msg, nullptr, 0, 0) > 0)
  {
    TranslateMessage(&msg);
    DispatchMessage(&msg);
  }
#endif
}