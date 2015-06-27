#include "logger.h"
#include <stdarg.h>
#include <string>

//=======================================================================
//=======================================================================
// init static member variables
Log::LogLevel Log::_logLevel = Log::LogLevel_Trace;
bool Log::_logXml = false;
FILE* Log::_fp = NULL;

//Log::setLogLevel(Log::LogLevel_Info);
//Log::setLogXml(false);

//=======================================================================
//=======================================================================
void Log::logMsg(const tchar *pacMsg)
{
    //OutputDebugString(pacMsg);
    printf(pacMsg);

    if (_fp)
    {
        fprintf(_fp, pacMsg);
    }
}

//=======================================================================
//=======================================================================
bool Log::shouldLog(LogLevel level)
{
    if ((int)level <= (int)_logLevel)
    {
        return true;
    }

    return false;
}

//=======================================================================
//=======================================================================
bool Log::logToFile(const char *file)
{
    if (_fp)
    {
        fclose(_fp);
        _fp = NULL;
    }

    fopen_s(&_fp, file, "wt");
    if (!_fp) return false;

     return true;
}

//=======================================================================
//=======================================================================
void Log::shutdown()
{
    if (_fp)
    {
        fclose(_fp);
        _fp = NULL;
    }
}

//=======================================================================
//=======================================================================
void LogError(const tchar *format, ...)
{
    if (!Log::shouldLog(Log::LogLevel_Error)) return;

    tchar buffer[10240];
    va_list args;

    va_start(args, format);
    vsprintf_s(buffer, 10240, format, args);
    va_end(args);

    std::string strOut;
    if (Log::getLogXml())
    {
        strOut = "<error>";
        strOut += buffer;
        strOut += "</error>\n";
    }
    else
    {
        strOut = "Error: ";
        strOut += buffer;
        strOut += "\n";
    }

    Log::logMsg(strOut.c_str());
}

//=======================================================================
//=======================================================================
void LogWarning(const tchar *format, ...)
{
    if (!Log::shouldLog(Log::LogLevel_Warning)) return;

    tchar buffer[10240];
    va_list args;

    va_start(args, format);
    vsprintf_s(buffer, 10240, format, args);
    va_end(args);

    std::string strOut;
    if (Log::getLogXml())
    {
        strOut = "<warning>";
        strOut += buffer;
        strOut += "</warning>\n";
    }
    else
    {
        strOut = "Warning: ";
        strOut += buffer;
        strOut += "\n";
    }

    Log::logMsg(strOut.c_str());
}

//=======================================================================
//=======================================================================
void LogInfo(const tchar *format, ...)
{
    if (!Log::shouldLog(Log::LogLevel_Info)) return;

    tchar buffer[10240];
    va_list args;

    va_start(args, format);
    vsprintf_s(buffer, 10240, format, args);
    va_end(args);

    std::string strOut;
    if (Log::getLogXml())
    {
        strOut = "<trace>";
        strOut += buffer;
        strOut += "</trace>\n";
    }
    else
    {
        strOut = buffer;
        strOut += "\n";
    }

    Log::logMsg(strOut.c_str());
}

//=======================================================================
//=======================================================================
void LogTrace(const tchar *format, ...)
{
    if (!Log::shouldLog(Log::LogLevel_Trace)) return;

    tchar buffer[10240];
    va_list args;

    va_start(args, format);
    vsprintf_s(buffer, 10240, format, args);
    va_end(args);

    std::string strOut;
    if (Log::getLogXml())
    {
        strOut = "<trace>";
        strOut += buffer;
        strOut += "</trace>\n";
    }
    else
    {
        strOut = buffer;
        strOut += "\n";
    }

    Log::logMsg(strOut.c_str());
}
