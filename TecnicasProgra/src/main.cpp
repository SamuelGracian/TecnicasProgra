
#include "Window.h"


int main()
{
  Window window;
  window.init(800,600, L"Tecnicas Progra");

  bool isAppRunning = true;
  while (isAppRunning)
  {
    window.processMessages();
  }

  return 1;
}