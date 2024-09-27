#include "Log.hpp"

#ifdef _WIN32
#include <Windows.h>
#endif

Log::Log() : mLogLevel{ NORMAL }
{
}

void Log::setLogLevel( LogLevel ll )
{
  mLogLevel = ll;
}

void Log::log( LogLevel ll, std::string const & message )
{
  if ( ll >= mLogLevel )
  {
    std::cout << message;
#ifdef _WIN32
    OutputDebugStringA( message.c_str() );
#endif
  }
}

Log & Log::instance()
{
  static Log instance{};
  return instance;
}

Formatter::Formatter( Log::LogLevel ll ) : mLl{ ll }, mSS{}
{
}

Formatter::~Formatter()
{
  mSS << std::endl;
  Log::instance().log( mLl, mSS.str() );
}
