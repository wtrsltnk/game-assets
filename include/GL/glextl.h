#ifndef GLEXTL_H
#define GLEXTL_H

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#define GL_GLEXT_PROTOTYPES
#include <GL/glext.h>
#ifdef _WIN32
#define WGL_WGLEXT_PROTOTYPES
#include <GL/wglext.h>
// On Windows we need the WINAPI call to make the functino pointer work
typedef void* (WINAPI PFNGLGETPROC)(const GLubyte* name);
#else
typedef void* (PFNGLGETPROC)(const GLubyte* name);
#endif

bool glExtLoadAll(PFNGLGETPROC* proc);
bool glExtLoadOne(PFNGLGETPROC* proc, const char* name);
bool glExtIsLoaded(const char* name);

#endif // GLEXTL_H