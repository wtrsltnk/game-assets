#include "settings.h"
#include <fstream>
#include <sstream>

Settings* Settings::_instance = nullptr;

Settings* Settings::Instance()
{
    if (Settings::_instance == nullptr)
        Settings::_instance = new Settings();

    return Settings::_instance;
}

void Settings::LoadFromDisk(const std::string& filename)
{
    this->_settings.clear();

    std::ifstream file(filename);
    std::string line;

    while (getline(file, line))
    {
        std::string::size_type found = line.find('=');
        if (found != std::string::npos)
            this->_settings.insert(std::make_pair(line.substr(0, found), line.substr(found + 1)));
    }
}

void Settings::SaveToDisk(const std::string& filename)
{
    std::ofstream file(filename);

    if (file.is_open())
    {
        for (auto itr =this->_settings.begin(); itr != this->_settings.end(); ++itr)
            file << itr->first << "=" << itr->second << "\n";
        file.close();
    }
}

Setting::Setting(const std::string& key)
    : _key(key)
{
    if (Settings::Instance()->_settings.find(key) != Settings::Instance()->_settings.end())
        this->_value = Settings::Instance()->_settings.at(key);
}

Setting& Setting::operator = (int value)
{
    std::stringstream ss;
    ss << value;
    (*this) = ss.str();

    return *this;
}

Setting& Setting::operator = (float value)
{
    std::stringstream ss;
    ss << value;
    (*this) = ss.str();

    return *this;
}

Setting& Setting::operator = (bool value)
{
    (*this) = std::string(value ? "true" : "false");

    return *this;
}

Setting& Setting::operator = (const std::string& value)
{
    this->_value = value;

    Settings::Instance()->_settings[this->_key] = this->_value;

    return *this;
}

Setting& Setting::Register(int defaultValue)
{
    if (Settings::Instance()->_settings.find(this->_key) == Settings::Instance()->_settings.end())
    {
        std::stringstream ss;
        ss << defaultValue;
        this->_value = ss.str();
        Settings::Instance()->_settings.insert(std::make_pair(this->_key, this->_value));
    }

    return *this;
}

Setting& Setting::Register(float defaultValue)
{
    if (Settings::Instance()->_settings.find(this->_key) == Settings::Instance()->_settings.end())
    {
        std::stringstream ss;
        ss << defaultValue;
        this->_value = ss.str();
        Settings::Instance()->_settings.insert(std::make_pair(this->_key, this->_value));
    }

    return *this;
}

Setting& Setting::Register(bool defaultValue)
{
    if (Settings::Instance()->_settings.find(this->_key) == Settings::Instance()->_settings.end())
    {
        this->_value = defaultValue ? "true" : "false";
        Settings::Instance()->_settings.insert(std::make_pair(this->_key, this->_value));
    }

    return *this;
}

Setting& Setting::Register(const std::string& defaultValue)
{
    if (Settings::Instance()->_settings.find(this->_key) == Settings::Instance()->_settings.end())
    {
        this->_value = defaultValue;
        Settings::Instance()->_settings.insert(std::make_pair(this->_key, this->_value));
    }

    return *this;
}

int Setting::AsInt() const
{
    return atoi(this->_value.c_str());
}

float Setting::AsFloat() const
{
    return atof(this->_value.c_str());
}

bool Setting::AsBool() const
{
    return this->_value == "true";
}

const std::string& Setting::AsString() const
{
    return this->_value;
}
