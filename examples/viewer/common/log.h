#ifndef LOG_H
#define LOG_H

#include <string>
#include <map>

class LoggingStrategy
{
public:
    virtual ~LoggingStrategy() { }

    virtual void LogVerbose(const std::string& message) = 0;
    virtual void LogInfo(const std::string& message) = 0;
    virtual void LogDebug(const std::string& message) = 0;
    virtual void LogWarning(const std::string& message) = 0;
    virtual void LogError(const std::string& message) = 0;
    virtual void LogException(const std::exception& exception) = 0;

};

class ConsoleLoggingStrategy : public LoggingStrategy
{
public:
    virtual ~ConsoleLoggingStrategy() { }

    virtual void LogVerbose(const std::string& message);
    virtual void LogInfo(const std::string& message);
    virtual void LogDebug(const std::string& message);
    virtual void LogWarning(const std::string& message);
    virtual void LogError(const std::string& message);
    virtual void LogException(const std::exception& exception);

};

class FileLoggingStrategy : public LoggingStrategy
{
    class FileLoggingStrategy_Impl* _impl;

public:
    FileLoggingStrategy();
    FileLoggingStrategy(const std::string& filename);
    virtual ~FileLoggingStrategy();

    virtual void LogVerbose(const std::string& message);
    virtual void LogInfo(const std::string& message);
    virtual void LogDebug(const std::string& message);
    virtual void LogWarning(const std::string& message);
    virtual void LogError(const std::string& message);
    virtual void LogException(const std::exception& exception);

};

class Logging
{
    friend class Log;
    static Logging* _instance;

    Logging();
    ConsoleLoggingStrategy _defaultStrategy;
    LoggingStrategy* _currentStrategy;

public:
    static Logging* Instance();
    void SetStrategy(LoggingStrategy* strategy);

    LoggingStrategy* CurrentStrategy() { return this->_currentStrategy; }
};

class Log
{
public:
    void Verbose(const std::string& message);
    void Info(const std::string& message);
    void Debug(const std::string& message);
    void Warning(const std::string& message);
    void Error(const std::string& message);
    void Exception(const std::exception& exception);
};

#endif // LOG_H
