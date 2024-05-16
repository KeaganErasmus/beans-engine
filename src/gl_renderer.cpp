#include "gl_renderer.h"
#include "beans_lib.h"
#include <glcorearb.h>
#include "input.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "render_interface.h"
#include <glcorearb.h>

const char* TEXTURE_PATH = "assets/textures/texture_atlas.png";

// Opengl Structs
struct GLContext {
    GLuint programID;
    GLuint textureID;
    GLuint transformSBOID;
    GLuint screenSizeID;
};

// opengl globals
static GLContext glContext;

// opengl Functions
static void APIENTRY gl_debug_callback(GLenum source, 
                                        GLenum type, 
                                        GLuint id, 
                                        GLenum severity, 
                                        GLsizei length, 
                                        const GLchar* message, 
                                        const void* user) 
{
    if(severity == GL_DEBUG_SEVERITY_LOW ||
       severity == GL_DEBUG_SEVERITY_MEDIUM ||
       severity == GL_DEBUG_SEVERITY_HIGH) {
        SM_ASSERT(false, "Opengl error %s ", message);
    }                                  
    else {
        SM_TRACE((char*)message);
    }
}


bool gl_init(BumpAllocator* transientStorage) {
    load_gl_functions();

    glDebugMessageCallback((GLDEBUGPROC)&gl_debug_callback, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glEnable(GL_DEBUG_OUTPUT);

    GLuint vertShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    int fileSize = 0;
    char* vertShader = read_file("assets/shaders/quad.vert", &fileSize, transientStorage);
    char* fragShader = read_file("assets/shaders/quad.frag", &fileSize, transientStorage);

    glShaderSource(vertShaderID, 1, &vertShader, 0);
    glShaderSource(fragShaderID, 1, &fragShader, 0);

    glCompileShader(vertShaderID);
    glCompileShader(fragShaderID);

    {
        int success;
        char shaderLog[2048] = {};

        glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &success);

        if(!success) {
            glGetShaderInfoLog(vertShaderID, 2048, 0, shaderLog);
            SM_ASSERT(false, "Failed to compile vertex shader %s ", shaderLog);
        }
    }

        {
        int success;
        char shaderLog[2048] = {};

        glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &success);

        if(!success) {
            glGetShaderInfoLog(fragShaderID, 2048, 0, shaderLog);
            SM_ASSERT(false, "Failed to compile frag shader %s ", shaderLog);
        }
    }

    glContext.programID = glCreateProgram();
    glAttachShader(glContext.programID, vertShaderID);
    glAttachShader(glContext.programID, fragShaderID);

    glLinkProgram(glContext.programID);

    glDetachShader(glContext.programID, vertShaderID);
    glDetachShader(glContext.programID, fragShaderID);
    glDeleteShader(vertShaderID);
    glDeleteShader(fragShaderID);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    {
        int width, height, channels;
        char* data = (char*)stbi_load(TEXTURE_PATH, &width, &height, &channels, 4);
        if(!data){
            SM_ASSERT(false, "Could not load file %s ", TEXTURE_PATH);
            return false;
        }

        glGenTextures(1, &glContext.textureID);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, glContext.textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }

    {
        glGenBuffers(1, &glContext.transformSBOID);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, glContext.transformSBOID);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Transform) * MAX_TRANSFORMS, renderData.transforms, GL_DYNAMIC_DRAW);
    }

    {
        glContext.screenSizeID = glGetUniformLocation(glContext.programID, "screenSize");
    }

    glEnable(GL_FRAMEBUFFER_SRGB);
    glEnable(0x809D); // Disable multisampling

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_GREATER);

    glUseProgram(glContext.programID);

    return true;
}

void gl_render() {
    glClearColor(24.0f / 255.0f, 24 / 255.0f, 24 / 255.0f, 1.0f);
    glClearDepth(0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, input.screenSizeX, input.screenSizeY);

    Vec2 screenSize = {(float)input.screenSizeX, (float)input.screenSizeY};
    glUniform2fv(glContext.screenSizeID, 1, &screenSize.x);

    {
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(Transform) * renderData.transformCount, renderData.transforms);

        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, renderData.transformCount);

        renderData.transformCount = 0;
    }
}