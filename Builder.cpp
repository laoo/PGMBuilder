#include "Builder.hpp"
#include "MameDB.hpp"
#include "Log.hpp"
#include "Ex.hpp"

struct BuilderData
{
};

Builder::Builder()
{
}

Builder::~Builder()
{
}

void Builder::declareROM( uint32_t crc, size_t size, char const* name )
{
  auto entry = mameDB::findGameEntry( crc );
  std::stringstream ss;
  ss << name << " -> { ";
  for ( auto const& pGE : entry )
  {
    ss << pGE->name << ' ';

    auto it = std::lower_bound( mSum.begin(), mSum.end(), pGE, mameDB::GameEntryComparer{} );
    if ( it == mSum.end() || ( *it )->name != pGE->name )
      mSum.insert( it, pGE );
  }
  ss << '}';
  LV << ss.str();

  mRoms.emplace_back( size, crc, std::move( entry ) );
}

void Builder::commit()
{
  auto intersection = computeIntersection();
  if ( intersection.size() != 1 )
  {
    std::stringstream ss;
    for ( auto const& s : mSum )
    {
      ss << s->name << ' ';
    }

    if ( intersection.size() == 0 )
      throw Ex{} << "Specified ROM files from disjoint sets of games: " << ss.str();
    else
      throw Ex{} << "Specified ROM files from ambigous set of games: " << ss.str();
  }

  auto gameEntry = intersection[0];

  LV << "intersection: " << gameEntry->name;

}

size_t Builder::addROM( uint32_t crc, size_t offset, const void* data, size_t size )
{
  return size;
}

void Builder::build( std::filesystem::path const& out )
{
}

std::vector<std::shared_ptr<mameDB::GameEntry>> Builder::computeIntersection() const
{
  std::vector<std::shared_ptr<mameDB::GameEntry>> intersection = mSum;

  for ( auto const& rom : mRoms )
  {
    std::vector<std::shared_ptr<mameDB::GameEntry>> intersection2;
    std::set_intersection( intersection.cbegin(), intersection.cend(), rom.games.begin(), rom.games.end(), std::back_inserter( intersection2 ), mameDB::GameEntryComparer{} );
    std::swap( intersection, intersection2 );
  }

  return intersection;
}
