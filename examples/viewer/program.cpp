#include "program.h"
#include "filesystem.h"
#include "common/settings.h"
#include "common/log.h"
#include <valve/hl1bspasset.h>
#include <valve/hl1bspinstance.h>
#include <valve/hl2bspasset.h>
#include <valve/hl2bspinstance.h>
#include <valve/hl1mdlasset.h>
#include <valve/hl1mdlinstance.h>
#include <valve/hl1mapasset.h>
#include <valve/hl1mapinstance.h>
#include <valve/hl1sprasset.h>
#include <valve/hl1sprinstance.h>
#include <SDL.h>
#include <iostream>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_transform.hpp>

Application* gApp = new AssetViewer();
static FileLoggingStrategy fileLogging;

AssetViewer::AssetViewer()
    : _pan(false), _lastX(0), _lastY(0), _asset(nullptr), _instance(nullptr)
{
    Settings::Instance()->LoadFromDisk("assetviewer.settings");
    Setting("Viewer.PauseAnimation").Register(false);
    Setting("Viewer.Camera.Speed").Register(200.0f);
    Logging::Instance()->SetStrategy(&fileLogging);
}

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
    glClearColor(0 / 255.0f, 129 / 255.0f, 210 / 255.0f, 255 / 255.0f);

    if (this->_sys->GetArgs().size() > 1)
    {
        std::string filename = this->_sys->GetArgs()[1];
        std::string ext = filename.substr(filename.length()-4);

        if (ext == ".bsp")
        {
            valve::Array<byte> sig = FileSystem::LoadPartialFileData(filename, 4);
            if (sig[0] == 'V' && sig[1] == 'B' && sig[2] == 'S' && sig[3] == 'P')
            {
                Log().Info("Loading Source BSP file");
                this->_asset = new valve::hl2::BspAsset(FileSystem::LocateDataFile, FileSystem::LoadFileData);
                if (this->_asset->Load(filename))
                    this->_instance = new valve::hl2::BspInstance((valve::hl2::BspAsset*)this->_asset);
            }
            else if (((int*)sig.data)[0] == HL1_BSP_SIGNATURE)
            {
                Log().Info("Loading GoldSrc BSP file");
                this->_asset = new valve::hl1::BspAsset(FileSystem::LocateDataFile, FileSystem::LoadFileData);
                if (this->_asset->Load(filename))
                    this->_instance = new valve::hl1::BspInstance((valve::hl1::BspAsset*)this->_asset);
            }
        }
        else if (ext == ".mdl")
        {
            Log().Info("Loading MDL file");
            this->_asset = new valve::hl1::MdlAsset(FileSystem::LocateDataFile, FileSystem::LoadFileData);
            if (this->_asset->Load(filename))
                this->_instance = new valve::hl1::MdlInstance((valve::hl1::MdlAsset*)this->_asset);
        }
        else if (ext == ".map")
        {
            Log().Info("Loading MAP file");
            this->_asset = new valve::hl1::MapAsset(FileSystem::LocateDataFile, FileSystem::LoadFileData);
            if (this->_asset->Load(filename))
                this->_instance = new valve::hl1::MapInstance((valve::hl1::MapAsset*)this->_asset);
        }
        else if (ext == ".spr")
        {
            Log().Info("Loading SPR file");
            this->_asset = new valve::hl1::SprAsset(FileSystem::LocateDataFile, FileSystem::LoadFileData);
            if (this->_asset->Load(filename))
                this->_instance = new valve::hl1::SprInstance((valve::hl1::SprAsset*)this->_asset);
        }

        if (this->_asset == nullptr || this->_instance == nullptr)
            Log().Info("Nothing loaded");

        this->_hud.InitHud(filename, this->_instance);
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
    double speed = double(Setting("Viewer.Camera.Speed").AsFloat());

    auto prevpos = this->_cam.Position();

    if (this->_sys->IsKeyDown(KeyCodes::Character_A)) this->_cam.MoveLeft(diff * speed);
    else if (this->_sys->IsKeyDown(KeyCodes::Character_D)) this->_cam.MoveLeft(-diff * speed);

    if (this->_sys->IsKeyDown(KeyCodes::Character_W)) this->_cam.MoveForward(diff * speed);
    else if (this->_sys->IsKeyDown(KeyCodes::Character_S)) this->_cam.MoveForward(-diff * speed);

    lastTime = time;

    double updateDiff = time - lastUpdateTime;
    if (updateDiff > 1.0/60.0)
    {
        if (this->_instance != nullptr && Setting("Viewer.PauseAnimation").AsBool() == false)
            this->_instance->Update(updateDiff);

        lastUpdateTime = time;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GEQUAL, 0.8f);

    if (this->_instance != nullptr)
        this->_instance->Render(this->_proj, this->_cam.GetViewMatrix());

    this->_hud.Render();
}

bool AssetViewer::IsRunning()
{
    const Uint8 *state = SDL_GetKeyboardState(nullptr);
    return !state[SDL_SCANCODE_ESCAPE];
}

void AssetViewer:: Resize(int w, int h)
{
    if (h < 1) h = 1;

    this->_hud.Resize(w, h);
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
{
    if (key == SDLK_SPACE && action) Setting("Viewer.PauseAnimation") = !Setting("Viewer.PauseAnimation").AsBool();
    else if (key == SDLK_KP_PLUS && action) Setting("Viewer.Camera.Speed") = Setting("Viewer.Camera.Speed").AsFloat() + 5.0f;
    else if (key == SDLK_KP_MINUS && action) Setting("Viewer.Camera.Speed") = Setting("Viewer.Camera.Speed").AsFloat() - 5.0f;
    else
        this->_hud.KeyAction(key, action);
}

void AssetViewer::Destroy()
{
    if (this->_instance != nullptr) delete this->_instance;
    if (this->_asset != nullptr) delete this->_asset;

    Settings::Instance()->SaveToDisk("assetviewer.settings");
}
