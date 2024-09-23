#include "MameDB.hpp"

namespace mameDB
{

int registerGame( RomEntry const* romEntry, char const* name, char const* fullName, char const* company, char const* year, AsicClass asicClass )
{
  static int cnt = 0;
  return cnt++;
}

}
