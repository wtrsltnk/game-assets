#include <GL/glextl.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <iostream>
#include "application.h"

using namespace std;

/// The keymapping is for SDL only
Uint8* keyMapping = new Uint8[KeyCodes::KeyCodesCount];
void InitKeyMapping();

/// Temporary key state buffer. This is updated every tick.
const Uint8* keyStates;

System::System(int argc, char *argv[])
{
    InitKeyMapping();
    for (int i = 0; i < argc; i++)
        this->_args.push_back(argv[i]);
}

System::~System()
{ }

double System::GetTime()
{
    return double(SDL_GetTicks()) / 1000.0;
}

bool System::IsKeyDown(unsigned int key)
{
    return (keyStates[keyMapping[key]] != 0);
}

class Window_SDL : public Application::Window
{
private:
    SDL_Window* _window;
    SDL_GLContext _context;

public:
    Window_SDL(const char* title, int width, int height)
        : Window(), _window(0), _context(0)
    {
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
        this->_window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | gApp->GetWindowFlags());
        if (this->_window != 0)
        {
            cout << "Window created" << endl;
            this->_context = SDL_GL_CreateContext(this->_window);
            if (this->_context == 0)
                cout << "Unable to create GL context..." << endl;
            else
                cout << "Context created" << endl;
        }
        else
            cout << "Unable to open Window..." << endl;
    }

    virtual ~Window_SDL()
    {
        SDL_GL_DeleteContext(this->_context);
        SDL_DestroyWindow(this->_window);
    }

    virtual void MakeCurrent()
    {
        SDL_GL_MakeCurrent(this->_window, this->_context);
    }

    virtual void Swap()
    {
        // Swap front and back rendering buffers
        SDL_GL_SwapWindow(this->_window);
    }

    bool IsValid()
    {
        return (this->_window != 0 && this->_context != 0);
    }

    SDL_Window* GetWindow() { return this->_window; }
};

Window_SDL* mainWindow = 0;

Application::Window* Application::MainWindow()
{
    return mainWindow;
}

Application::Window* Application::AddWindow(const char* title, int width, int height)
{
    return new Window_SDL(title, width, height);
}

void Application::CloseWindow(Application::Window* window)
{
    delete window;
}

int main(int argc, char* argv[])
{
    System sys(argc, argv);
    if (gApp != 0)
    {
        cout << "App Ok" << endl;
        if (gApp->InitializeApplication(&sys))
        {
            cout << "Application initialized" << endl;
            if (gApp->IsCommandLineApplication())
            {
                while (gApp->IsRunning())
                {
                    gApp->GameLoop();
                }
                gApp->Destroy();
            }
            else
            {
                if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) == 0)
                {
                    int major, minor;
                    bool core;
                    gApp->GetContextAttributes(major, minor, core);
                    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
                    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
                    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, core ? SDL_GL_CONTEXT_PROFILE_CORE : 0);

                    int width = 1280, height = 720;
                    mainWindow = new Window_SDL(gApp->GetWindowTitle(), width, height);
                    if (mainWindow->IsValid())
                    {
                        glExtLoadAll((PFNGLGETPROC*)&SDL_GL_GetProcAddress);
                        cout << "Functions loaded" << endl;
                        if (gApp->InitializeGraphics())
                        {
                            gApp->Resize(width, height);

                            SDL_Event event;
                            cout << "Graphics initialized" << endl;
                            while (gApp->IsRunning())
                            {
                                while (SDL_PollEvent(&event))
                                {
                                    if (event.type == SDL_MOUSEMOTION)
                                    {
                                        gApp->MouseMove(event.motion.x, event.motion.y);
                                    }
                                    else if (event.type == SDL_MOUSEBUTTONDOWN)
                                    {
                                        gApp->MouseButtonDown(event.button.button, event.button.x, event.button.y);
                                    }
                                    else if (event.type == SDL_MOUSEBUTTONUP)
                                    {
                                        gApp->MouseButtonUp(event.button.button, event.button.x, event.button.y);
                                    }
                                    else if (event.type == SDL_MOUSEWHEEL)
                                    {
                                        gApp->MouseWheel(event.wheel.x, event.wheel.y);
                                    }
                                    else if (event.type == SDL_KEYDOWN)
                                    {
                                        gApp->KeyAction(event.key.keysym.sym, event.key.state);
                                    }
                                    else if (event.type == SDL_KEYUP)
                                    {
                                        gApp->KeyAction(event.key.keysym.sym, event.key.state);
                                    }
                                    else if (event.type == SDL_WINDOWEVENT)
                                    {
                                        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                                            gApp->Resize(event.window.data1, event.window.data2);
                                    }
                                }
                                SDL_PumpEvents();
                                keyStates = SDL_GetKeyboardState(NULL);
                                gApp->GameLoop();

                                // Swap front and back rendering buffers
                                mainWindow->Swap();
                            }
                            gApp->Destroy();
                        }
                        else
                        {
                            cout << "Unable to initialize graphics..." << endl;
                        }
                        delete mainWindow;
                    }
                    SDL_Quit();
                }
                else
                {
                    cout << "Unable to initialize SDL..." << endl;
                }
            }
        }
        else
        {
            cout << "Unable to initialize Application..." << endl;
        }
    }
    else
    {
        cout << "No application defined." << endl;
    }
    return 0;
}

void InitKeyMapping()
{
    keyMapping[KeyCodes::Return] = SDL_SCANCODE_RETURN;
    keyMapping[KeyCodes::Escape] = SDL_SCANCODE_ESCAPE;
    keyMapping[KeyCodes::Backspace] = SDL_SCANCODE_BACKSPACE;
    keyMapping[KeyCodes::Tab] = SDL_SCANCODE_TAB;
    keyMapping[KeyCodes::Character_Space] = SDL_SCANCODE_SPACE;

    keyMapping[KeyCodes::Character_A] = SDL_SCANCODE_A;
    keyMapping[KeyCodes::Character_B] = SDL_SCANCODE_B;
    keyMapping[KeyCodes::Character_C] = SDL_SCANCODE_C;
    keyMapping[KeyCodes::Character_D] = SDL_SCANCODE_D;
    keyMapping[KeyCodes::Character_E] = SDL_SCANCODE_E;
    keyMapping[KeyCodes::Character_F] = SDL_SCANCODE_F;
    keyMapping[KeyCodes::Character_G] = SDL_SCANCODE_G;
    keyMapping[KeyCodes::Character_H] = SDL_SCANCODE_H;
    keyMapping[KeyCodes::Character_I] = SDL_SCANCODE_I;
    keyMapping[KeyCodes::Character_J] = SDL_SCANCODE_J;
    keyMapping[KeyCodes::Character_K] = SDL_SCANCODE_K;
    keyMapping[KeyCodes::Character_L] = SDL_SCANCODE_L;
    keyMapping[KeyCodes::Character_M] = SDL_SCANCODE_M;
    keyMapping[KeyCodes::Character_N] = SDL_SCANCODE_N;
    keyMapping[KeyCodes::Character_O] = SDL_SCANCODE_O;
    keyMapping[KeyCodes::Character_P] = SDL_SCANCODE_P;
    keyMapping[KeyCodes::Character_Q] = SDL_SCANCODE_Q;
    keyMapping[KeyCodes::Character_R] = SDL_SCANCODE_R;
    keyMapping[KeyCodes::Character_S] = SDL_SCANCODE_S;
    keyMapping[KeyCodes::Character_T] = SDL_SCANCODE_T;
    keyMapping[KeyCodes::Character_U] = SDL_SCANCODE_U;
    keyMapping[KeyCodes::Character_V] = SDL_SCANCODE_V;
    keyMapping[KeyCodes::Character_W] = SDL_SCANCODE_W;
    keyMapping[KeyCodes::Character_X] = SDL_SCANCODE_X;
    keyMapping[KeyCodes::Character_Y] = SDL_SCANCODE_Y;
    keyMapping[KeyCodes::Character_Z] = SDL_SCANCODE_Z;
}
