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

  void indent() { mIndent++; }
  void outdent() { if (mIndent) mIndent--; }

private:
  Log();

  LogLevel mLogLevel;
  int mIndent;
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
#define LOGINDENT ::Log::instance().indent()
#define LOGOUTDENT ::Log::instance().outdent()

#define LOGLEVEL(LL) ::Log::instance().setLogLevel( LL );
