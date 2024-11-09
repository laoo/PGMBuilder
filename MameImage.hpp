#pragma once

#include "Image.hpp"
#include "RawROM.hpp"

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

public:
  MameImage( std::shared_ptr<GameEntry> aGameEntry, std::vector<ROMSlot> aSlots );
};

}
