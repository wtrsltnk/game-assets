#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>
#include <string>

class System
{
public:
    System(int argc, char* argv[]);
    virtual ~System();

    /// Gets a vector with the application execution arguments
    std::vector<std::string>& GetArgs() { return _args; }

    /// Gets the time since application start, 1.0 = 1 second
    double GetTime();

    /// Gets if a key is down
    bool IsKeyDown(unsigned int key);
private:
    std::vector<std::string> _args;

};

class Application
{
public:
    class Window
    {
    protected:
        Window() { }
    public:
        virtual ~Window() { }

        virtual void MakeCurrent() = 0;
        virtual void Swap() = 0;
    };

public:
    virtual ~Application() { }

    virtual const char* GetWindowTitle() { return "OpenGL 3"; }
    virtual void GetContextAttributes(int& major, int& minor, bool& core) = 0;
    virtual int GetWindowFlags() { return 0; }
    virtual bool IsCommandLineApplication() { return false; }

    virtual bool InitializeApplication(System* sys) = 0;
    virtual bool InitializeGraphics() = 0;
    virtual void GameLoop() = 0;
    virtual bool IsRunning() = 0;
    virtual void Resize(int w, int h) = 0;
    virtual void MouseMove(int x, int y) = 0;
    virtual void MouseButtonDown(int button, int x, int y) = 0;
    virtual void MouseButtonUp(int button, int x, int y) = 0;
    virtual void MouseWheel(int x, int y) = 0;
    virtual void KeyAction(int key, int action) = 0;
    virtual void Close() = 0;
    virtual void Destroy() = 0;

    Window* MainWindow();
    Window* AddWindow(const char* title, int width, int height);
    void CloseWindow(Window* window);

};

extern Application* gApp;

namespace KeyCodes
{
enum eKeyCodes
{
    Unknown = 0,
    Return,
    Escape,
    Backspace,
    Tab,
    Character_Space,

    Character_A,
    Character_B,
    Character_C,
    Character_D,
    Character_E,
    Character_F,
    Character_G,
    Character_H,
    Character_I,
    Character_J,
    Character_K,
    Character_L,
    Character_M,
    Character_N,
    Character_O,
    Character_P,
    Character_Q,
    Character_R,
    Character_S,
    Character_T,
    Character_U,
    Character_V,
    Character_W,
    Character_X,
    Character_Y,
    Character_Z,



    KeyCodesCount   /// Numer of key codes, for array bounds
};
}

#endif // APPLICATION_H
