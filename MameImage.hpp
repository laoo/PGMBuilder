#pragma once

#include "Image.hpp"
#include "RawROM.hpp"
#include "pgm.hpp"
#include "generator.hpp"

namespace mameDB
{
struct GameEntry;

class MameImage : public Image
{
public:
  ~MameImage() override;
  void addROM( std::shared_ptr<RawROM> rom ) override;
  bool isComplete() const override;
  void build( std::filesystem::path const& out ) const override;

  static std::shared_ptr<MameImage> create( std::string const& tpl );

private:

  enum struct RomType
  {
    NONE,
    P,
    T,
    M,
    B,
    A
  };

  struct RomOp
  {
    uint32_t offset = 0;
    uint32_t length = 0;
    uint32_t flags = 0;
  };

  struct ROMSlot
  {
    RomType type;
    uint32_t crc;
    std::shared_ptr<RawROM> src = {};
    std::vector<RomOp> ops;
  };

  std::shared_ptr<GameEntry> mGameEntry;
  std::vector<ROMSlot> mSlots;

  struct RomAssembly
  {
    uint32_t begin;
    uint32_t end;
    std::vector<uint8_t> data;

    RomAssembly( uint32_t beg, uint32_t end );
    void add( RomOp const& op, RawROM const& );
  };

  RomAssembly assembleROM( RomType type ) const;

  cppcoro::generator<ROMSlot const&> slotsByType( RomType type ) const;

public:
  MameImage( std::shared_ptr<GameEntry> aGameEntry, std::vector<ROMSlot> aSlots );
};

}
