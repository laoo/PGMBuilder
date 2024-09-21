#pragma once

namespace cxxopts
{
  class Options;
  class ParseResult;
}

class ProgramOptions
{
public:
  ProgramOptions( int argc, char const* argv[] );
  ProgramOptions( ProgramOptions const& ) = delete;
  ProgramOptions& operator=( ProgramOptions const& ) = delete;

  std::filesystem::path input() const;
  std::filesystem::path output() const;


private:
  std::shared_ptr<cxxopts::Options> mOpt;
  std::shared_ptr<cxxopts::ParseResult> mRes;

  std::filesystem::path mInput = {};
  std::filesystem::path mOutput = {};
};
