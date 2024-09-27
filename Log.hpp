#pragma once

#include <string>
#include <sstream>

class Log
{
public:
  enum LogLevel
  {
    VERBOSE,
    NORMAL,
    QUIET
  };

  void setLogLevel( LogLevel ll );

  void log( LogLevel ll, std::string const& message );

  static Log & instance();

private:
  Log();

  LogLevel mLogLevel;
};

class Formatter
{
public:
  Formatter( Log::LogLevel ll );
  ~Formatter();

  template<typename T>
  Formatter & operator<<( T const& t )
  {
    mSS << t;
    return *this;
  }

private:
  Log::LogLevel mLl;
  std::stringstream mSS;
};

#define LV ::Formatter{ ::Log::VERBOSE }
#define LN ::Formatter{ ::Log::NORMAL }

#define LOGLEVEL(LL) ::Log::instance().setLogLevel( LL );
