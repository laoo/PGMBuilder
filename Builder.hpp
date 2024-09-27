#pragma once

namespace mameDB
{
struct GameEntry;
}

struct BuilderData;


class Builder
{
public:
  Builder();
  ~Builder();

  void declareROM( uint32_t crc, size_t size, char const* name );
  void commit();
  size_t addROM( uint32_t crc, size_t offset, const void* data, size_t size );
  void build( std::filesystem::path const& out );

private:
  std::vector<std::shared_ptr<mameDB::GameEntry>> computeIntersection() const;

private:
  struct ROMEntry
  {
    size_t size;
    uint32_t crc;
    std::span<std::shared_ptr<mameDB::GameEntry>> games;
  };

  std::vector<ROMEntry> mRoms = {};
  std::vector<std::shared_ptr<mameDB::GameEntry>> mSum = {};

};
