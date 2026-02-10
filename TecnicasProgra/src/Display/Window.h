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

#if defined (_WIN32)

  inline HWND GetHandle() const { return m_hwnd; }
#endif

 private:

#if defined(_WIN32)
  HWND m_hwnd;
#endif
};