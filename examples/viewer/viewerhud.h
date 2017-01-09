#ifndef VIEWERHUD_H
#define VIEWERHUD_H

#include "common/font.h"
#include <valve/hltypes.h>
#include <valve/hl1bspinstance.h>
#include <valve/hl1mdlinstance.h>
#include <valve/hl1sprinstance.h>
#include <valve/hl2bspinstance.h>
#include <string>
#include <glm/glm.hpp>

class IHud
{
public:
    virtual ~IHud() { }

    virtual void KeyAction(int key, int action) { }

    virtual void Render(const glm::mat4& proj, const glm::mat4& view) = 0;

    virtual std::string AdditionalHelp() const { return std::string(); }
};

class ViewerHud
{
public:
    struct eFonts {
        Font Regular;
        Font Info;

    } Fonts;

public:
    ViewerHud();
    virtual ~ViewerHud();

    void Resize(int w, int h);
    virtual void KeyAction(int key, int action);

    void InitHud(const std::string& filename, valve::AssetInstance* instance);
    void Render();

    const glm::vec2& Size() const { return this->_size; }

private:
    glm::mat4 _proj;
    glm::vec2 _size;

    std::string _filename;

    valve::hl1::BspInstance* _hl1Bsp;
    valve::hl1::MdlInstance* _hl1Mdl;
    valve::hl1::SprInstance* _hl1Spr;
    valve::hl2::BspInstance* _hl2Bsp;

    IHud* _hud;

};

class Hl1BspHud : public IHud
{
    valve::hl1::BspInstance* _instance;
    ViewerHud& _mainHud;

public:
    Hl1BspHud(valve::hl1::BspInstance* instance, ViewerHud& mainHud);
    virtual ~Hl1BspHud() { }

    virtual void Render(const glm::mat4& proj, const glm::mat4& view);

};

class Hl1MdlHud : public IHud
{
    valve::hl1::MdlInstance* _instance;
    ViewerHud& _mainHud;

public:
    Hl1MdlHud(valve::hl1::MdlInstance* instance, ViewerHud& mainHud);
    virtual ~Hl1MdlHud() { }

    virtual void Render(const glm::mat4& proj, const glm::mat4& view);

};

class Hl1SprHud : public IHud
{
    valve::hl1::SprInstance* _instance;
    ViewerHud& _mainHud;

public:
    Hl1SprHud(valve::hl1::SprInstance* instance, ViewerHud& mainHud);
    virtual ~Hl1SprHud() { }

    virtual void Render(const glm::mat4& proj, const glm::mat4& view);

};

class Hl2BspHud : public IHud
{
    valve::hl2::BspInstance* _instance;
    ViewerHud& _mainHud;

public:
    Hl2BspHud(valve::hl2::BspInstance* instance, ViewerHud& mainHud);
    virtual ~Hl2BspHud() { }

    virtual void Render(const glm::mat4& proj, const glm::mat4& view);

};

#endif // VIEWERHUD_H
