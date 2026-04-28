#include "WriteUtils.hpp"

size_t round512( std::ofstream& fout )
{
  size_t p = fout.tellp();
  if ( ( p & 511 ) != 0 )
  {
    fout.seekp( ( p & ~511ull ) + 512ull );
  }

  return fout.tellp();
}
