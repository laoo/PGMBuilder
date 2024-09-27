#include "Builder.hpp"
#include "MameDB.hpp"
#include "Log.hpp"

struct BuilderData
{
  struct ROMEntry
  {
    size_t size;
    uint32_t crc;
    std::span<std::shared_ptr<mameDB::GameEntry>> games;
  };

  std::vector<ROMEntry> roms;
  std::vector<std::shared_ptr<mameDB::GameEntry>> sum;
};

Builder::Builder() : mData{ std::make_unique<BuilderData>() }
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

    auto it = std::lower_bound( mData->sum.begin(), mData->sum.end(), pGE, mameDB::GameEntryComparer{} );
    if ( it == mData->sum.end() || ( *it )->name != pGE->name )
      mData->sum.insert( it, pGE );
  }
  ss << '}';
  LV << ss.str();

  mData->roms.emplace_back( size, crc, std::move( entry ) );
}

void Builder::commit()
{
  auto intersection = computeIntersection();
  LV << "intersection: " << intersection[0]->name;
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
  std::vector<std::shared_ptr<mameDB::GameEntry>> intersection = mData->sum;

  for ( auto const& rom : mData->roms )
  {
    std::vector<std::shared_ptr<mameDB::GameEntry>> intersection2;
    std::set_intersection( intersection.cbegin(), intersection.cend(), rom.games.begin(), rom.games.end(), std::back_inserter( intersection2 ), mameDB::GameEntryComparer{} );
    std::swap( intersection, intersection2 );
  }

  return intersection;
}
