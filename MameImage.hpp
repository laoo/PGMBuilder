#pragma once

#include "Image.hpp"
#include "RawROM.hpp"

namespace mameDB
{
struct GameEntry;

class MameImage : public Image
{
public:
  MameImage( std::string const& tpl );
  ~MameImage() override;

  void addROM( std::shared_ptr<RawROM> rom ) override;

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
  std::vector<ROMSlot> mSlots = {};
};

}
