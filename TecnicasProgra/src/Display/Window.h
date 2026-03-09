#pragma once

#if defined(_WIN32)
#include <windows.h>
#endif


class DisplaySurface 
{
 public:
	DisplaySurface() 
		: m_width(0)
		, m_height(0)
#if defined (_WIN32)
		,m_hwnd (nullptr)
#endif
	{}

  ~DisplaySurface();

  bool init(int width, int height, wchar_t* windowName);

  void processMessages();

  int GetHeight() const { return m_height; }

  int GetWidth() const { return m_width; }

  int GetClientWidth()  const { return m_clientWidth; }

  int GetClientHeight() const { return m_clientHeight; }

#if defined (_WIN32)

  inline HWND GetHandle() const { return m_hwnd; }
#endif

 private:
	int m_width, m_height;
	int m_clientWidth, m_clientHeight;
#if defined(_WIN32)
  HWND m_hwnd;
#endif
};