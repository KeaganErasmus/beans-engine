#include "beans_lib.h"
#include "input.h"
#include "platform.h"

#define APIENTRY
#define GL_GLEXT_PROTOTYPES
#include "glcorearb.h"

//Windows layer
#ifdef _WIN32
#include "win32_platform.cpp"
#endif

#include "gl_renderer.cpp"


int main () {
    BumpAllocator transientStorage = make_bump_allocator(MB(50));
    platform_create_window(1200, 720, "Celeste");

    input.screenSizeX = 1200;
    input.screenSizeY = 720;

    gl_init(&transientStorage);

   while (running)
   {
        platform_update_window();
        gl_render();
        
        platform_swap_buffers();
   }
   
}