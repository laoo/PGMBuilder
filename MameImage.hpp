#pragma once

#include "Image.hpp"
#include "RomAssembly.hpp"
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

  struct ROMSlot
  {
    RomType type;
    uint32_t crc;
    std::shared_ptr<RawROM> src = {};
    std::vector<RomOp> ops;
  };

  std::shared_ptr<GameEntry> mGameEntry;
  std::vector<ROMSlot> mSlots;


  RomAssembly assembleROM( RomType type ) const;

  cppcoro::generator<ROMSlot const&> slotsByType( RomType type ) const;

public:
  MameImage( std::shared_ptr<GameEntry> aGameEntry, std::vector<ROMSlot> aSlots );
};

}
