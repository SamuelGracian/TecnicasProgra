#pragma once

#if defined(_WIN32)
#include <windows.h>
#endif


class DisplaySurface 
{
 public:
  DisplaySurface() = default;
  ~DisplaySurface();

  bool init(int width, int height, wchar_t* windowName);

  void processMessages();

 private:

#if defined(_WIN32)
  HWND m_hwnd;
#endif
};