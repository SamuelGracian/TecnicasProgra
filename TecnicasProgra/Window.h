
#ifdef WIN32
#include <windows.h>
#endif


class Window {
 public:
  Window() = default;
  ~Window();

  bool init(int width, int height, wchar_t* windowName);

  void processMessages();

 private:

#ifdef WIN32
  HWND m_hwnd;
#endif
};