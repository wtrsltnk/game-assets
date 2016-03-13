#ifndef VIEWERHUD_H
#define VIEWERHUD_H

#include "common/font.h"
#include <hl1types.h>
#include <hl1bspinstance.h>
#include <hl1mdlinstance.h>
#include <hl1sprinstance.h>
#include <hl2bspinstance.h>
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

    void InitHud(const std::string& filename, Hl1Instance* instance);
    void Render();

    const glm::vec2& Size() const { return this->_size; }

private:
    glm::mat4 _proj;
    glm::vec2 _size;

    std::string _filename;

    Hl1BspInstance* _hl1Bsp;
    Hl1MdlInstance* _hl1Mdl;
    Hl1SprInstance* _hl1Spr;
    Hl2BspInstance* _hl2Bsp;

    IHud* _hud;

};

class Hl1BspHud : public IHud
{
    Hl1BspInstance* _instance;
    ViewerHud& _mainHud;

public:
    Hl1BspHud(Hl1BspInstance* instance, ViewerHud& mainHud);
    virtual ~Hl1BspHud() { }

    virtual void Render(const glm::mat4& proj, const glm::mat4& view);

};

class Hl1MdlHud : public IHud
{
    Hl1MdlInstance* _instance;
    ViewerHud& _mainHud;

public:
    Hl1MdlHud(Hl1MdlInstance* instance, ViewerHud& mainHud);
    virtual ~Hl1MdlHud() { }

    virtual void Render(const glm::mat4& proj, const glm::mat4& view);

};

class Hl1SprHud : public IHud
{
    Hl1SprInstance* _instance;
    ViewerHud& _mainHud;

public:
    Hl1SprHud(Hl1SprInstance* instance, ViewerHud& mainHud);
    virtual ~Hl1SprHud() { }

    virtual void Render(const glm::mat4& proj, const glm::mat4& view);

};

class Hl2BspHud : public IHud
{
    Hl2BspInstance* _instance;
    ViewerHud& _mainHud;

public:
    Hl2BspHud(Hl2BspInstance* instance, ViewerHud& mainHud);
    virtual ~Hl2BspHud() { }

    virtual void Render(const glm::mat4& proj, const glm::mat4& view);

};

#endif // VIEWERHUD_H
