#include <iostream>
#include "beans_lib.h"
#include "platform.h"

//Windows layer
#ifdef _WIN32
#include "win32_platform.cpp"
#endif

#include "gl_renderer.h"

int main () {
    platform_create_window(1200, 720, "Celeste");

   while (running)
   {
        platform_update_window();
   }
   
}