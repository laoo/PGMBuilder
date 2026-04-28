#pragma once

#include "pgm.hpp"

namespace cxxopts
{
  class Options;
  class ParseResult;
}

class ProgramOptions
{
public:
  struct ManualRomArg
  {
    std::filesystem::path path;
    uint32_t mapping = 0;
  };

  struct ManualInfo
  {
    std::string manufacturer;
    std::string shortName;
    std::string asciiLongName;
    std::string utf8LongName;
    std::string year;
    pgm::AsicClass hardware = pgm::pgm_state;
  };

  ProgramOptions( int argc, char const* argv[] );
  ProgramOptions( ProgramOptions const& ) = delete;
  ProgramOptions& operator=( ProgramOptions const& ) = delete;

  bool isRomMode() const;
  std::filesystem::path input() const;
  std::filesystem::path output() const;
  std::optional<ManualRomArg> const& romPrg() const;
  std::optional<ManualRomArg> const& romInt() const;
  std::optional<ManualRomArg> const& romExt() const;
  std::optional<ManualRomArg> const& romTle() const;
  std::optional<ManualRomArg> const& romSpm() const;
  std::optional<ManualRomArg> const& romSpc() const;
  std::optional<ManualRomArg> const& romAud() const;
  ManualInfo const& manualInfo() const;


private:
  std::shared_ptr<cxxopts::Options> mOpt;
  std::shared_ptr<cxxopts::ParseResult> mRes;

  bool mRomMode = false;
  std::filesystem::path mInput = {};
  std::filesystem::path mOutput = {};

  std::optional<ManualRomArg> mRomPrg = {};
  std::optional<ManualRomArg> mRomInt = {};
  std::optional<ManualRomArg> mRomExt = {};
  std::optional<ManualRomArg> mRomTle = {};
  std::optional<ManualRomArg> mRomSpm = {};
  std::optional<ManualRomArg> mRomSpc = {};
  std::optional<ManualRomArg> mRomAud = {};

  ManualInfo mManualInfo = {};
};
