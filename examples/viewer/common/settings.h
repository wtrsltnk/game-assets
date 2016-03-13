#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <map>

class Settings
{
    friend class Setting;
    std::map<std::string, std::string> _settings;
    static Settings* _instance;

public:
    static Settings* Instance();

    void LoadFromDisk(const std::string& filename);
    void SaveToDisk(const std::string& filename);
};

class Setting
{
public:
    Setting(const std::string& key);

    Setting& Register(int defaultValue);
    Setting& Register(float defaultValue);
    Setting& Register(bool defaultValue);
    Setting& Register(const std::string& defaultValue);

    Setting& operator = (int value);
    Setting& operator = (float value);
    Setting& operator = (bool value);
    Setting& operator = (const std::string& value);

    int AsInt() const;
    float AsFloat() const;
    bool AsBool() const;
    const std::string& AsString() const;

private:
    std::string _key;
    std::string _value;

};

#endif // SETTINGS_H
