#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>

#ifndef tchar
    #define tchar char
#endif

void LogError(const tchar *format, ...);
void LogWarning(const tchar *format, ...);
void LogInfo(const tchar *format, ...);
void LogTrace(const tchar *format, ...);

class Log
{
public:
    enum LogLevel
    {
        LogLevel_Error = 1,
        LogLevel_Warning = 2,
        LogLevel_Info = 3,
        LogLevel_Trace = 4
    };

    static void logMsg(const tchar *pacMsg);

    static bool logToFile(const char *file);
    static void setLogLevel(LogLevel level) { _logLevel = level; }
    static LogLevel getLogLevel() { return _logLevel; }

    static void setLogXml(bool logxml) { _logXml = logxml; }
    static bool getLogXml() { return _logXml; }

    static bool shouldLog(LogLevel level);

    static void shutdown();

protected:
    static LogLevel _logLevel;
    static bool _logXml;
    static FILE *_fp;
};

#endif
