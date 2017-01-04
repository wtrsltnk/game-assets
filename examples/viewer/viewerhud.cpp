#include "viewerhud.h"
#include "common/settings.h"
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <SDL.h>

/// HL1 BSP
Hl1BspHud::Hl1BspHud(valve::hl1::BspInstance *instance, ViewerHud& mainHud)
    : _instance(instance), _mainHud(mainHud)
{ }

void Hl1BspHud::Render(const glm::mat4 &proj, const glm::mat4 &view)
{ }

/// HL1 MDL
Hl1MdlHud::Hl1MdlHud(valve::hl1::MdlInstance *instance, ViewerHud& mainHud)
    : _instance(instance), _mainHud(mainHud)
{ }

void Hl1MdlHud::Render(const glm::mat4 &proj, const glm::mat4 &view)
{
    if (Setting("Hud.ShowInfo").AsBool())
    {
        std::stringstream ss;
        ss << "[ INFO ]" << std::endl <<
              "Sequence count : " << this->_instance->Asset()->SequenceCount() << std::endl <<
              "Bodypart count : " << this->_instance->Asset()->BodypartCount() << std::endl;
        this->_mainHud.Fonts.Info.DrawTextA(proj, view, this->_mainHud.Size().x - 300.0f, 20.0f, ss.str());
    }
}

/// HL1 SPR
Hl1SprHud::Hl1SprHud(valve::hl1::SprInstance *instance, ViewerHud& mainHud)
    : _instance(instance), _mainHud(mainHud)
{ }

void Hl1SprHud::Render(const glm::mat4 &proj, const glm::mat4 &view)
{
    if (Setting("Hud.ShowInfo").AsBool())
    {
        std::stringstream ss;
        ss << "[ INFO ]" << std::endl <<
              "  Frame count : " << this->_instance->Asset()->FrameCount();
        this->_mainHud.Fonts.Info.DrawTextA(proj, view, this->_mainHud.Size().x - 300.0f, 20.0f, ss.str());
    }
}

/// HL2 BSP
Hl2BspHud::Hl2BspHud(valve::hl2::BspInstance *instance, ViewerHud& mainHud)
    : _instance(instance), _mainHud(mainHud)
{ }

void Hl2BspHud::Render(const glm::mat4 &proj, const glm::mat4 &view)
{ }

ViewerHud::ViewerHud()
    : _hl1Bsp(nullptr), _hl1Mdl(nullptr), _hl1Spr(nullptr), _hl2Bsp(nullptr), _hud(nullptr)
{
    Setting("Hud.ShowHelp").Register(true);
    Setting("Hud.ShowInfo").Register(false);
}

ViewerHud::~ViewerHud()
{ }

void ViewerHud::Resize(int w, int h)
{
    this->_size = glm::vec2(float(w), float(h));
    this->_proj = glm::ortho(0.0f, this->_size.x, this->_size.y, 0.0f);
}

void ViewerHud::KeyAction(int key, int action)
{
    if (key == SDLK_i && action) Setting("Hud.ShowInfo") = !Setting("Hud.ShowInfo").AsBool();
    else if (key == SDLK_h && action) Setting("Hud.ShowHelp") = !Setting("Hud.ShowHelp").AsBool();
    else if (this->_hud != nullptr)
        this->_hud->KeyAction(key, action);
}

void ViewerHud::InitHud(const std::string& filename, valve::AssetInstance* instance)
{
    this->Fonts.Regular.InitializeFont("c:\\windows\\fonts\\verdana.ttf");
    this->Fonts.Info.InitializeFont("c:\\windows\\fonts\\cour.ttf", 18.0f);

    this->_filename = filename;

    this->_hl1Bsp = dynamic_cast<valve::hl1::BspInstance*>(instance);
    if (this->_hl1Bsp != nullptr)
    {
        this->_hud = new Hl1BspHud(this->_hl1Bsp, *this);
        return;
    }

    this->_hl1Mdl = dynamic_cast<valve::hl1::MdlInstance*>(instance);
    if (this->_hl1Mdl != nullptr)
    {
        this->_hud = new Hl1MdlHud(this->_hl1Mdl, *this);
        return;
    }

    this->_hl1Spr = dynamic_cast<valve::hl1::SprInstance*>(instance);
    if (this->_hl1Spr != nullptr)
    {
        this->_hud = new Hl1SprHud(this->_hl1Spr, *this);
        return;
    }

    this->_hl2Bsp = dynamic_cast<valve::hl2::BspInstance*>(instance);
    if (this->_hl2Bsp != nullptr)
    {
        this->_hud = new Hl2BspHud(this->_hl2Bsp, *this);
        return;
    }
}

void ViewerHud::Render()
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_ALPHA_TEST);
    if (this->_hud != nullptr)
    {
        this->Fonts.Regular.DrawTextA(this->_proj, glm::mat4(1.0f), 5.0f, this->_size.y - 5.0f, (std::string("Loaded file : ") + this->_filename));
        std::stringstream speed;
        speed << "Camera speed : " << Setting("Viewer.Camera.Speed").AsFloat();
        this->Fonts.Regular.DrawTextA(this->_proj, glm::mat4(1.0f), this->_size.x - 130.0f, this->_size.y - 5.0f, speed.str());

        if (Setting("Hud.ShowHelp").AsBool())
        {
            std::stringstream ss;
            ss << "[ HELP ]" << std::endl <<
                  "  <SPACE>    - Pause/unpause animation" << std::endl <<
                  "  h          - Toggle this help on/off" << std::endl <<
                  "  i          - Toggle asset info on/off" << std::endl <<
                  "  <KP_PLUS>  - Increase camera speed" << std::endl <<
                  "  <KP_MINUS> - Decrease camera speed" << std::endl << this->_hud->AdditionalHelp();
            this->Fonts.Info.DrawTextA(this->_proj, glm::mat4(1.0f), 10.0f, 20.0f, ss.str());

        }
        this->_hud->Render(this->_proj, glm::mat4(1.0f));
    }
    else
    {
        this->Fonts.Regular.DrawTextA(this->_proj, glm::mat4(1.0f), 5.0f, this->_size.y - 5.0f, (std::string("Nothing loaded")));
    }
}
