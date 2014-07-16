#ifndef LOGGER_H
#define LOGGER_H

#ifndef tchar
    #define tchar char
#endif

void LogError(const tchar *format, ...);
void LogWarning(const tchar *format, ...);
void LogTrace(const tchar *format, ...);
void LogMsg(const tchar *pacMsg);

#endif
