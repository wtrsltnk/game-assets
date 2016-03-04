#include "program.h"
#include "filesystem.h"
#include <hl1bspinstance.h>
#include <hl2bspinstance.h>
#include <hl1mdlinstance.h>
#include <hl1mapinstance.h>
#include <GL/glextl.h>
#include <SDL.h>
#include <iostream>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

Application* gApp = new AssetViewer();

AssetViewer::AssetViewer()
    : _pan(false), _lastX(0), _lastY(0), _asset(nullptr), _instance(nullptr)
{ }

AssetViewer::~AssetViewer()
{ }

bool AssetViewer::InitializeApplication(System* sys)
{
    this->_sys = sys;

    return true;
}

bool AssetViewer::InitializeGraphics()
{
    std::cout << "GL_VERSION                  : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GL_SHADING_LANGUAGE_VERSION : " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "GL_RENDERER                 : " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "GL_VENDOR                   : " << glGetString(GL_VENDOR) << std::endl;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    if (this->_sys->GetArgs().size() > 1)
    {
        std::string filename = this->_sys->GetArgs()[1];
        std::string ext = filename.substr(filename.length()-4);

        if (ext == ".bsp")
            this->_asset = new Hl1BspAsset(FileSystem::LoadFileData);
        else if (ext == ".mdl")
            this->_asset = new Hl1MdlAsset(FileSystem::LoadFileData);
        else if (ext == ".map")
            this->_asset = new Hl1MapAsset(FileSystem::LoadFileData);

        if (this->_asset != nullptr
                && this->_asset->Load(filename))
            this->_instance = this->_asset->CreateInstance();
    }

    this->_cam.MoveForward(-120.0f);

    return true;
}

void AssetViewer::GameLoop()
{
    static double lastTime = this->_sys->GetTime();
    static double lastUpdateTime = this->_sys->GetTime();

    double time = this->_sys->GetTime();
    double diff = time - lastTime;
    double speed = 200.0;

    if (this->_sys->IsKeyDown(KeyCodes::Character_A)) this->_cam.MoveLeft(diff * speed);
    else if (this->_sys->IsKeyDown(KeyCodes::Character_D)) this->_cam.MoveLeft(-diff * speed);

    if (this->_sys->IsKeyDown(KeyCodes::Character_W)) this->_cam.MoveForward(diff * speed);
    else if (this->_sys->IsKeyDown(KeyCodes::Character_S)) this->_cam.MoveForward(-diff * speed);

    lastTime = time;

    double updateDiff = time - lastUpdateTime;
    if (updateDiff > 1.0/60.0)
    {
        if (this->_instance != nullptr)
            this->_instance->Update(updateDiff);

        lastUpdateTime = time;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    if (this->_instance != nullptr)
        this->_instance->Render(this->_proj, this->_cam.GetViewMatrix());
}

bool AssetViewer::IsRunning()
{
    const Uint8 *state = SDL_GetKeyboardState(nullptr);
    return !state[SDL_SCANCODE_ESCAPE];
}

void AssetViewer:: Resize(int w, int h)
{
    if (h < 1) h = 1;

    glViewport(0, 0, w, h);

    this->_proj = glm::perspective(120.0f, float(w) / float(h), 0.1f, 4000.0f);
}

void AssetViewer::MouseButtonDown(int button, int x, int y)
{
    this->_pan = (button == SDL_BUTTON_LEFT);
    this->_lastX = x;
    this->_lastY = y;
}

void AssetViewer::MouseMove(int x, int y)
{
    if (this->_pan)
    {
        this->_cam.RotateZ(glm::radians(float(this->_lastX-x) * 0.1f));
        this->_cam.RotateX(glm::radians(float(this->_lastY-y) * 0.1f));
    }

    this->_lastX = x;
    this->_lastY = y;
}

void AssetViewer::MouseButtonUp(int button, int x, int y)
{
    this->_pan = false;
}

void AssetViewer::MouseWheel(int x, int y)
{ }

void AssetViewer::KeyAction(int key, int action)
{ }

void AssetViewer::Destroy()
{
    if (this->_instance != nullptr) delete this->_instance;
    if (this->_asset != nullptr) delete this->_asset;
}