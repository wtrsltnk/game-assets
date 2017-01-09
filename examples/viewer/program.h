#ifndef HL1BSPVIEWER_H
#define HL1BSPVIEWER_H

#include "viewerhud.h"
#include "common/application.h"
#include "common/camera.h"
#include <valve/hltypes.h>
#include <glm/glm.hpp>

class AssetViewer : public Application
{
    System* _sys;
    valve::Asset* _asset;
    valve::AssetInstance* _instance;
    glm::mat4 _proj;
    int _lastX, _lastY;
    bool _pan;
    Camera _cam;
    ViewerHud _hud;

public:
    AssetViewer();
    virtual ~AssetViewer();

public:
    virtual const char* GetWindowTitle() { return "asset-viewer"; }
    virtual void GetContextAttributes(int& major, int& minor, bool& core) { major = 3; minor = 0; core = false; }

    virtual bool InitializeApplication(System* sys);
    virtual bool InitializeGraphics();
    virtual void GameLoop();
    virtual bool IsRunning();
    virtual void Resize(int w, int h);
    virtual void MouseMove(int x, int y);
    virtual void MouseButtonDown(int button, int x, int y);
    virtual void MouseButtonUp(int button, int x, int y);
    virtual void MouseWheel(int x, int y);
    virtual void KeyAction(int key, int action);
    virtual void Close() { }
    virtual void Destroy();

};

#endif // HL1BSPVIEWER_H
