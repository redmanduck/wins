#ifndef __LOG_H__
#define __LOG_H__

#include <sstream>
#include <string>
#include <stdio.h>

inline std::string NowTime();

enum TLogLevel {
  logCRITICAL     = 0x0,
  logERROR        = 0x1,
  logWARNING      = 0x2,
  logINFO         = 0x4,
  logIMU          = 0x8,
  logLOCATION     = 0x10,
  logSPI          = 0x20,
  logDISPLAY      = 0x40,
  logWIFI         = 0x80,
  logKEYPAD       = 0x100,
  logDEBUG        = 0x200,
  logDEBUG1       = 0x400,
  logDEBUG2       = 0x800,
  logDEBUG3       = 0x1000,
  logDEBUG4       = 0x2000
};

template <typename T>
class Log
{
public:
    Log();
    virtual ~Log();
    std::ostringstream& Get(TLogLevel level = logINFO);
public:
    static TLogLevel& ReportingLevel();
    static TLogLevel& LogSelect();
    static std::string ToString(TLogLevel level);
    static TLogLevel FromString(const std::string& level);
protected:
    std::ostringstream os;
private:
    Log(const Log&);
    Log& operator =(const Log&);
};

template <typename T>
Log<T>::Log()
{
}

template <typename T>
std::ostringstream& Log<T>::Get(TLogLevel level)
{
    os << "- " << NowTime();
    os << " " << ToString(level) << ": ";
    os << std::string(level > logDEBUG ? level - logDEBUG : 0, '\t');
    return os;
}

template <typename T>
Log<T>::~Log()
{
    os << std::endl;
    T::Output(os.str());
}

template <typename T>
TLogLevel& Log<T>::ReportingLevel()
{
    static TLogLevel reportingLevel = logCRITICAL;
    return reportingLevel;
}

template <typename T>
TLogLevel& Log<T>::LogSelect() {
    static TLogLevel logSelect = logCRITICAL;
    return logSelect;
}

template <typename T>
std::string Log<T>::ToString(TLogLevel level)
{
    if (level == logDEBUG4)
        return "DEBUG4";
    if (level == logDEBUG3)
        return "DEBUG3";
    if (level == logDEBUG2)
        return "DEBUG2";
    if (level == logDEBUG1)
        return "DEBUG1";
    if (level == logDEBUG)
        return "DEBUG";
    if (level == logKEYPAD)
        return "KEYPAD";
    if (level == logWIFI)
        return "WIFI";
    if (level == logDISPLAY)
        return "DISPLAY";
    if (level == logSPI)
        return "SPI";
    if (level == logLOCATION)
        return "LOCATION";
    if (level == logIMU)
        return "IMU";
    if (level == logINFO)
        return "INFO";
    if (level == logWARNING)
        return "WARNING";
    if (level == logERROR)
        return "ERROR";
    if (level == logCRITICAL)
        return "CRITICAL";
    return "UNKOWN LOG LEVEL";
}

template <typename T>
TLogLevel Log<T>::FromString(const std::string& level)
{
    if (level == "DEBUG4")
        return logDEBUG4;
    if (level == "DEBUG3")
        return logDEBUG3;
    if (level == "DEBUG2")
        return logDEBUG2;
    if (level == "DEBUG1")
        return logDEBUG1;
    if (level == "DEBUG")
        return logDEBUG;
    if (level == "KEYPAD")
        return logKEYPAD;
    if (level == "WIFI")
        return logWIFI;
    if (level == "DISPLAY")
        return logDISPLAY;
    if (level == "SPI")
        return logSPI;
    if (level == "LOCATION")
        return logLOCATION;
    if (level == "IMU")
        return logIMU;
    if (level == "INFO")
        return logINFO;
    if (level == "WARNING")
        return logWARNING;
    if (level == "ERROR")
        return logERROR;
    if (level == "CRITICAL")
        return logCRITICAL;
    Log<T>().Get(logWARNING) << "Unknown logging level '" << level << "'. Using INFO level as default.";
    return logINFO;
}

class FStream {
 public:
  FILE* pStream;
  FStream() {
    pStream = fopen("wins.log", "a");
  }
  ~FStream() {
    fclose(pStream);
  }
};

class Output2FILE
{
public:
    static FILE*& Stream();
    static void Output(const std::string& msg);
};

inline FILE*& Output2FILE::Stream()
{
    static FStream file;
    return file.pStream;
}

inline void Output2FILE::Output(const std::string& msg)
{
    FILE* pStream = Stream();
    if (!pStream)
        return;
    fprintf(pStream, "%s", msg.c_str());
    fflush(pStream);
    printf("%s", msg.c_str());
    fflush(stdout);
}

#define LOG_SELECT (logKEYPAD | logDISPLAY)

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#   if defined (BUILDING_FILELOG_DLL)
#       define FILELOG_DECLSPEC   __declspec (dllexport)
#   elif defined (USING_FILELOG_DLL)
#       define FILELOG_DECLSPEC   __declspec (dllimport)
#   else
#       define FILELOG_DECLSPEC
#   endif // BUILDING_DBSIMPLE_DLL
#else
#   define FILELOG_DECLSPEC
#endif // _WIN32

class FILELOG_DECLSPEC FILELog : public Log<Output2FILE> {};
//typedef Log<Output2FILE> FILELog;

#ifndef FILELOG_MAX_LEVEL
#define FILELOG_MAX_LEVEL logDEBUG4
#endif

#define FILE_LOG(level) \
    if (level > FILELOG_MAX_LEVEL) ;\
    else if ((level > FILELog::ReportingLevel() || !Output2FILE::Stream()) \
              && !(level & FILELog::LogSelect())) ; \
    else FILELog().Get(level)

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

#include <windows.h>

inline std::string NowTime()
{
    const int MAX_LEN = 200;
    char buffer[MAX_LEN];
    if (GetTimeFormatA(LOCALE_USER_DEFAULT, 0, 0,
            "HH':'mm':'ss", buffer, MAX_LEN) == 0)
        return "Error in NowTime()";

    char result[100] = {0};
    static DWORD first = GetTickCount();
    std::sprintf(result, "%s.%03ld", buffer, (long)(GetTickCount() - first) % 1000);
    return result;
}

#else

#include <sys/time.h>

inline std::string NowTime()
{
    char buffer[11];
    time_t t;
    time(&t);
    tm r = {0};
    strftime(buffer, sizeof(buffer), "%X", localtime_r(&t, &r));
    struct timeval tv;
    gettimeofday(&tv, 0);
    char result[100] = {0};
    std::sprintf(result, "%s.%03ld", buffer, (long)tv.tv_usec / 1000);
    return result;
}

#endif //WIN32

#endif //__LOG_H__

