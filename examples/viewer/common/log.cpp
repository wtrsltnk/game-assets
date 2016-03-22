#include "log.h"
#include "settings.h"
#include <iostream>
#include <fstream>

void ConsoleLoggingStrategy::LogVerbose(const std::string& message) { std::cout << "[VERBOSE] " << message << std::endl; }
void ConsoleLoggingStrategy::LogInfo(const std::string& message) { std::cout << "[INFO] " << message << std::endl; }
void ConsoleLoggingStrategy::LogDebug(const std::string& message) { std::cout << "[DEBUG] " << message << std::endl; }
void ConsoleLoggingStrategy::LogWarning(const std::string& message) { std::cout << "[WARNING] " << message << std::endl; }
void ConsoleLoggingStrategy::LogError(const std::string& message) { std::cout << "[ERROR] " << message << std::endl; }
void ConsoleLoggingStrategy::LogException(const std::exception& exception) { std::cout << "[EXCEPTION] " << exception.what() << std::endl; }

class FileLoggingStrategy_Impl
{
public:
    FileLoggingStrategy_Impl(FileLoggingStrategy* parent, const std::string& filename)
        : _parent(parent), _stream(std::ofstream(filename))
    { }

    FileLoggingStrategy* _parent;
    std::ofstream _stream;
};

FileLoggingStrategy::FileLoggingStrategy(const std::string& filename)
    : _impl(new FileLoggingStrategy_Impl(this, filename))
{ }

FileLoggingStrategy::FileLoggingStrategy()
    : _impl(nullptr)
{
    Setting("Logging.Filename").Register("default.log");
    this->_impl = new FileLoggingStrategy_Impl(this, Setting("Logging.Filename").AsString());
}

FileLoggingStrategy::~FileLoggingStrategy()
{
    _impl->_stream.close();
}

void FileLoggingStrategy::LogVerbose(const std::string& message) { _impl->_stream << "[VERBOSE] " << message << std::endl; }
void FileLoggingStrategy::LogInfo(const std::string& message) { _impl->_stream << "[INFO] " << message << std::endl; }
void FileLoggingStrategy::LogDebug(const std::string& message) { _impl->_stream << "[DEBUG] " << message << std::endl; }
void FileLoggingStrategy::LogWarning(const std::string& message) { _impl->_stream << "[WARNING] " << message << std::endl; }
void FileLoggingStrategy::LogError(const std::string& message) { _impl->_stream << "[ERROR] " << message << std::endl; }
void FileLoggingStrategy::LogException(const std::exception& exception) { _impl->_stream << "[EXCEPTION] " << exception.what() << std::endl; }

Logging* Logging::_instance = nullptr;

Logging::Logging()
    : _currentStrategy(&_defaultStrategy)
{ }

Logging* Logging::Instance()
{
    if (Logging::_instance == nullptr)
        Logging::_instance = new Logging();

    return Logging::_instance;
}

void Logging::SetStrategy(LoggingStrategy* strategy)
{
    if (strategy != nullptr)
        this->_currentStrategy = strategy;
}

void Log::Verbose(const std::string& message) { Logging::Instance()->CurrentStrategy()->LogVerbose(message); }
void Log::Info(const std::string& message) { Logging::Instance()->CurrentStrategy()->LogInfo(message); }
void Log::Debug(const std::string& message) { Logging::Instance()->CurrentStrategy()->LogDebug(message); }
void Log::Warning(const std::string& message) { Logging::Instance()->CurrentStrategy()->LogWarning(message); }
void Log::Error(const std::string& message) { Logging::Instance()->CurrentStrategy()->LogError(message); }
void Log::Exception(const std::exception& exception) { Logging::Instance()->CurrentStrategy()->LogException(exception); }
